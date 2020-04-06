#-------------------------------------------------
# Filename : env.pri
# Description: set config library environment
# Author : lxw
# Created : 2019-07-06
#-------------------------------------------------
win32 {
    QWT_INSTALL_DIR = "D:/Qt/qwt-6.0.1"
    ZLIB_INSTALL_DIR = "D:/zlib"
    EIGEN_INSTALL_DIR = "D:/Eigen"
    NETCDF_INSTALL_DIR = "D:/netCDF"
    TINYXML_INSTALL_DIR = "D:/tinyxml2"
    RAPIDJSON_INSTALL_DIR = "D:/RapidJSON"
}

unix {
    QWT_INSTALL_DIR = "/usr/local/qwt-6.0.1-arm"
}
