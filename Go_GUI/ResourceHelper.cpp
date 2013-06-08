#include "ResourceHelper.hpp"

namespace Go_GUI {

QString generateResourcePath(QString resource) {
    if (!resource.startsWith('/') && !resource.startsWith('\\'))
        resource.prepend('/');
    return QCoreApplication::applicationDirPath() + resource;
}

} // namespace Go_GUI