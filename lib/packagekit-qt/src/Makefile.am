include ${top_srcdir}/moc.mk

NULL =

INCLUDES = \
	$(QTCORE_CFLAGS)					\
	$(QTDBUS_CFLAGS)					\
	$(POLKIT_CFLAGS)					\
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\"

METASOURCES = AUTO

lib_LTLIBRARIES =						\
	libpackagekit-qt.la					\
	$(NULL)

libpackagekit_qt_includedir = $(includedir)/PackageKit/packagekit-qt

libpackagekit_qt_include_HEADERS =				\
	QPackageKit						\
	client.h						\
	transaction.h						\
	package.h						\
	$(NULL)

$(libpackagekit_qt_la_OBJECTS) :				\
	client.moc						\
	clientprivate.moc					\
	daemonproxy.moc						\
	package.moc						\
	polkitclient.moc					\
	transaction.moc						\
	transactionprivate.moc					\
	transactionproxy.moc					\
	$(NULL)

libpackagekit_qt_la_SOURCES =					\
	QPackageKit						\
	common.h						\
	client.h						\
	client.cpp						\
	clientprivate.h						\
	clientprivate.cpp					\
	daemonproxy.h						\
	daemonproxy.cpp						\
	transaction.h						\
	transaction.cpp						\
	transactionprivate.h					\
	transactionprivate.cpp					\
	transactionproxy.h					\
	transactionproxy.cpp					\
	package.h						\
	package.cpp						\
	polkitclient.h						\
	polkitclient.cpp					\
	util.h							\
	util.cpp						\
	$(NULL)

libpackagekit_qt_la_LIBADD =					\
	$(QTCORE_LIBS)						\
	$(QTDBUS_LIBS)						\
	$(POLKIT_LIBS)						\
	$(NULL)

libpackagekit_qt_la_LDFLAGS =					\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)	\
	-export-dynamic						\
	-no-undefined						\
	$(NULL)

CLEANFILES = $(BUILT_SOURCES)

clean-local:
	rm -f *~
	rm -f $(CLEANFILES)

EXTRA_DIST =							\
	CMakeLists.txt						\
	$(NULL)
