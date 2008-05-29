/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2007 Tom Parker <palfrey@tevp.net>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <glib.h>
#include <glib/gstdio.h>
#include "pk-sqlite-pkg-cache.h"

static sqlite3 *db = NULL;
static PkBackend *backend;
static gboolean(*is_installed) (const PkPackageId *) = NULL;

void sqlite_set_installed_check(gboolean(*func) (const PkPackageId *))
{
	is_installed = func;
}

void
sqlite_init_cache(PkBackend *backend, const char* dbname, const char *compare_fname, void (*build_db)(PkBackend *, sqlite3 *))
{
	int ret;
	struct stat st;
	time_t db_age;

	ret = sqlite3_open (dbname, &db);
	g_assert(ret == SQLITE_OK);
	g_assert(db!=NULL);
	ret = sqlite3_exec(db,"PRAGMA synchronous = OFF",NULL,NULL,NULL);
	g_assert(ret == SQLITE_OK);

	g_stat(dbname, &st);
	db_age = st.st_mtime;
	g_stat(compare_fname, &st);
	if (db_age>=st.st_mtime)
	{
		ret = sqlite3_exec(db, "select value from params where name = 'build_complete'", NULL, NULL, NULL);
		if (ret != SQLITE_ERROR)
			return;
		pk_debug("ages are %lu for db, and %lu for comparism",db_age,st.st_mtime);
	}
	ret = sqlite3_exec(db,"drop table packages",NULL,NULL,NULL); // wipe it!
	//g_assert(ret == SQLITE_OK);
	pk_debug("wiped db");
	ret = sqlite3_exec(db,"create table packages (name text, version text, deps text, arch text, short_desc text, long_desc text, repo string, primary key(name,version,arch,repo))",NULL,NULL,NULL);
	g_assert(ret == SQLITE_OK);

	build_db(backend,db);

	sqlite3_exec(db,"create table params (name text primary key, value integer)", NULL, NULL, NULL);
	sqlite3_exec(db,"insert into params values ('build_complete',1)", NULL, NULL, NULL);
}

void sqlite_finish_cache(PkBackend *backend)
{
	sqlite3_close(db);
}

// sqlite_search_packages_thread
static gboolean
sqlite_search_packages_thread (PkBackend *backend)
{
	int res;
	gchar *sel;
	const gchar *search;

	pk_backend_set_status(backend, PK_STATUS_ENUM_QUERY);
	pk_backend_set_percentage (backend, PK_BACKEND_PERCENTAGE_INVALID);
	type = pk_backend_get_uint (backend, "type");
	search = pk_backend_get_string (backend, "search");

	pk_debug("finding %s", search);

	sqlite3_stmt *package = NULL;
	g_strdelimit(search," ",'%');

	if (type == SEARCH_NAME)
		sel = g_strdup_printf("select name,version,arch,repo,short_desc from packages where name like '%%%s%%'",search);
	else if (type == SEARCH_DETAILS)
		sel = g_strdup_printf("select name,version,arch,repo,short_desc from packages where name like '%%%s%%' or short_desc like '%%%s%%' or long_desc like '%%%s%%'",search, search, search);
	else
	{
		pk_backend_error_code(backend, PK_ERROR_ENUM_INTERNAL_ERROR, "Unknown search task type");
		goto end_search_packages;
	}

	pk_debug("statement is '%s'",sel);
	res = sqlite3_prepare_v2(db,sel, -1, &package, NULL);
	g_free(sel);
	if (res!=SQLITE_OK)
		pk_error("sqlite error during select prepare: %s", sqlite3_errmsg(db));
	res = sqlite3_step(package);
	while (res == SQLITE_ROW)
	{
		PkPackageId *pid = pk_package_id_new_from_list((const gchar*)sqlite3_column_text(package,0),
				(const gchar*)sqlite3_column_text(package,1),
				(const gchar*)sqlite3_column_text(package,2),
				(const gchar*)sqlite3_column_text(package,3));

		gchar *cpid = pk_package_id_to_string(pid);
		PkInfoEnum pie = PK_INFO_ENUM_UNKNOWN;

		if (is_installed != NULL)
			pie = is_installed(pid)?PK_INFO_ENUM_INSTALLED:PK_INFO_ENUM_AVAILABLE;

		pk_backend_package(backend, pie, cpid, (const gchar*)sqlite3_column_text(package,4));

		g_free(cpid);
		pk_package_id_free(pid);

		if (res==SQLITE_ROW)
			res = sqlite3_step(package);
	}
	if (res!=SQLITE_DONE)
	{
		pk_debug("sqlite error during step (%d): %s", res, sqlite3_errmsg(db));
		g_assert(0);
	}

end_search_packages:
	pk_backend_finished (backend);
	return TRUE;
}

/**
 * sqlite_search_details:
 */
void
sqlite_search_details (PkBackend *backend, const gchar *filter, const gchar *search)
{
	pk_backend_set_uint (backend, "type", SEARCH_DETAILS);
	pk_backend_thread_create (backend, sqlite_search_packages_thread);
}

/**
 * sqlite_search_name:
 */
void
sqlite_search_name (PkBackend *backend, const gchar *filter, const gchar *search)
{
	pk_backend_set_uint (backend, "type", SEARCH_NAME);
	pk_backend_thread_create (backend, sqlite_search_packages_thread);
}

// sqlite_get_details_thread
static gboolean
sqlite_get_details_thread (PkBackend *backend)
{
	PkPackageId *pi;
	const gchar *package_id;
	int res;

	package_id = pk_backend_get_string (backend, "package_id");
	pi = pk_package_id_new_from_string(package_id);
	if (pi == NULL)
	{
		pk_backend_error_code(backend, PK_ERROR_ENUM_PACKAGE_ID_INVALID, "invalid package id");
		pk_backend_finished(backend);
		return;
	}

	pk_backend_set_status(backend, PK_STATUS_ENUM_QUERY);
	pk_backend_set_percentage (backend, PK_BACKEND_PERCENTAGE_INVALID);

	pk_debug("finding %s", pi->name);

	sqlite3_stmt *package = NULL;
	gchar *sel = g_strdup_printf("select long_desc from packages where name = '%s' and version = '%s' and repo = '%s'",pi->name,pi->version,pi->data);
	pk_debug("statement is '%s'",sel);
	res = sqlite3_prepare_v2(db,sel, -1, &package, NULL);
	g_free(sel);
	if (res!=SQLITE_OK)
		pk_error("sqlite error during select prepare: %s", sqlite3_errmsg(db));
	res = sqlite3_step(package);
	pk_backend_details(backend,pi->name, "unknown", PK_GROUP_ENUM_OTHER,(const gchar*)sqlite3_column_text(package,0),"",0);
	res = sqlite3_step(package);
	if (res==SQLITE_ROW)
		pk_error("multiple matches for that package!");
	if (res!=SQLITE_DONE)
	{
		pk_debug("sqlite error during step (%d): %s", res, sqlite3_errmsg(db));
		g_assert(0);
	}

	g_free(dt);

	return TRUE;
}

/**
 * sqlite_get_details:
 */
extern "C++" void
sqlite_get_details (PkBackend *backend, const gchar *package_id)
{
	pk_backend_thread_create (backend, sqlite_get_details_thread);
	return;
}
