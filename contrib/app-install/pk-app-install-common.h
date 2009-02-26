/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2009 Richard Hughes <richard@hughsie.com>
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

#ifndef __PK_APP_INSTALL_COMMON_H
#define __PK_APP_INSTALL_COMMON_H

#if PK_BUILD_LOCAL
#define PK_APP_INSTALL_DEFAULT_DATABASE		"./desktop.db"
#define PK_APP_INSTALL_DEFAULT_ICONDIR		"./icons"
#else
#define PK_APP_INSTALL_DEFAULT_DATABASE		DATADIR "/app-install/cache/desktop.db"
#define PK_APP_INSTALL_DEFAULT_ICONDIR		DATADIR "/app-install/icons"
#endif

#endif /* __PK_APP_INSTALL_COMMON_H */