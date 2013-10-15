
# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
# CONFIG += mobility
# MOBILITY +=

SOURCES += main.cpp mainwindow.cpp \
    SocamPropertyStream.cpp
HEADERS += mainwindow.h \
    SocamPropertyStream.h
FORMS += mainwindow.ui


INSTALLS = target
target.path = /usr/bin
