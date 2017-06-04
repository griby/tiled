/*
 * GMapText Tiled Plugin
 * Copyright 2017, Guillaume Riby <guillaumeriby@gmail.com>
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "gmaptextplugin.h"

#include "tile.h"
#include "mapobject.h"

#include <QFileInfo>

using namespace Tiled;
using namespace GMapText;

GMapTextPlugin::GMapTextPlugin()
{
}

bool GMapTextPlugin::write(const Map *map, const QString &fileName)
{
    SaveFile file(fileName);

    // Open the file
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        mError = tr("Could not open file for writing.");
        return false;
    }

    auto fileDevice = file.device();

    // Write the map
    QFileInfo fileInfo(fileName);
    QString mapName = fileInfo.baseName();
    writeMap(map, mapName, fileDevice);

    // Check for errors
    if (file.error() != QFileDevice::NoError) {
        mError = file.errorString();
        return false;
    }

    // Save the file
    if (!file.commit()) {
        mError = file.errorString();
        return false;
    }

    return true;
}

void GMapTextPlugin::writeMap(const Map *map, const QString &mapName, QFileDevice *fileDevice)
{
    // Write the map header comment
    QString mapHeader = QString("# Map %1\n")
            .arg(mapName);
    fileDevice->write(mapHeader.toLatin1());

    // Write the map properties
    Tileset *tileset = nullptr;
    if (!map->tilesets().empty())
        tileset = map->tilesetAt(0).data();

    QString mapProperties = QString("%1 %2 %3 %4 %5\n")
            .arg(tileset->name())
            .arg(QString::number(map->width()))
            .arg(QString::number(map->height()))
            .arg(QString::number(map->tileWidth()))
            .arg(QString::number(map->tileHeight()));
    fileDevice->write(mapProperties.toLatin1());

    // Write the tile layers
    for (const TileLayer *tileLayer : map->tileLayers()) {
        fileDevice->write("\n", 1);
        writeTileLayer(tileLayer, fileDevice);
    }

    // Write the object groups
    for (const ObjectGroup *objectGroup : map->objectGroups()) {
        fileDevice->write("\n", 1);
        writeObjectGroup(objectGroup, fileDevice);
    }
}

void GMapTextPlugin::writeTileLayer(const TileLayer *tileLayer, QFileDevice *fileDevice)
{
    // Write the tile layer header comment
    QString tileLayerHeader = QString("# TileLayer %1\n")
            .arg(tileLayer->name());
    fileDevice->write(tileLayerHeader.toLatin1());

    // Write the tiles
    for (int y = 0; y < tileLayer->height(); ++y) {
        for (int x = 0; x < tileLayer->width(); ++x) {
            if (x > 0)
                fileDevice->write(" ", 1);

            // Write the tile ID or -1 if it's empty
            const Cell &cell = tileLayer->cellAt(x, y);
            const Tile *tile = cell.tile();
            const int id = tile ? tile->id() : -1;
            fileDevice->write(QByteArray::number(id));
        }

        fileDevice->write("\n", 1);
    }
}

void GMapTextPlugin::writeObjectGroup(const ObjectGroup *objectGroup, QFileDevice *fileDevice)
{
    // Write the object group header comment
    QString objectGroupHeader = QString("# ObjectGroup %1\n")
            .arg(objectGroup->name());
    fileDevice->write(objectGroupHeader.toLatin1());

    // Write the objects
    for (const MapObject *object : objectGroup->objects()) {
        writeObject(object, fileDevice);
    }
}

void GMapTextPlugin::writeObject(const MapObject *object, QFileDevice *fileDevice)
{
    // Write the object properties
    QString objectProperties = QString("%1 %2 %3 %4 %5 %6")
            .arg(object->name())
            .arg(object->type())
            .arg(QString::number(object->x()))
            .arg(QString::number(object->y()))
            .arg(QString::number(object->width()))
            .arg(QString::number(object->height()));
    fileDevice->write(objectProperties.toLatin1());

    // Write the custom properties
    Properties customProperties = object->properties();
    Properties::const_iterator it = customProperties.constBegin();
    for (; it != customProperties.constEnd(); ++it) {
        QString customProperty = QString(" %1 %2")
                .arg(it.key())
                .arg(it.value().toString());
        fileDevice->write(customProperty.toLatin1());
    }

    fileDevice->write("\n", 1);
}

QString GMapTextPlugin::nameFilter() const
{
    return tr("GMap text files (*.gmt)");
}

QString GMapTextPlugin::shortName() const
{
    return QLatin1String("gmaptext");
}

QString GMapTextPlugin::errorString() const
{
    return mError;
}
