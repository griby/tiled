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
    writeMap(map, fileDevice);

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

void GMapTextPlugin::writeMap(const Map *map, QFileDevice *fileDevice)
{
    // Write the map properties
    fileDevice->write("# Map properties\n");
    QString mapSize =
            QString("Size %1 %2\n")
            .arg(QString::number(map->width()), QString::number(map->height()));
    fileDevice->write(mapSize.toLatin1().constData());

    QString tileSize =
            QString("TileSize %1 %2\n")
            .arg(QString::number(map->tileWidth()), QString::number(map->tileHeight()));
    fileDevice->write(tileSize.toLatin1().constData());

    if (!map->tilesets().empty()) {
        Tileset *tileset = map->tilesetAt(0).data();
        QString tilesetName = QString("Tileset %1\n").arg(tileset->name());
        fileDevice->write(tilesetName.toLatin1().constData());
    }

    fileDevice->write("\n", 1);

    // Write the tile layers
    for (int i = 0; i < map->tileLayers().count(); ++i) {
        if (i > 0)
            fileDevice->write("\n", 1);

        const TileLayer *tileLayer = map->tileLayers().at(i);
        writeTileLayer(tileLayer, fileDevice);
    }

    // Write the object groups
    for (int i = 0; i < map->objectGroups().count(); ++i) {
        if (i > 0 || !map->tileLayers().empty())
            fileDevice->write("\n", 1);

        const ObjectGroup *objectGroup = map->objectGroups().at(i);
        writeObjectGroup(objectGroup, fileDevice);
    }
}

void GMapTextPlugin::writeTileLayer(const TileLayer *tileLayer, QFileDevice *fileDevice)
{
    // Write the tile layer properties
    fileDevice->write("# TileLayer properties\n");
    QString tileLayerName = QString("Name %1\n").arg(tileLayer->name());
    fileDevice->write(tileLayerName.toLatin1().constData());

    fileDevice->write("\n", 1);

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
    // Write the object group properties
    fileDevice->write("# ObjectGroup properties\n");
    QString objectGroupName = QString("Name %1\n").arg(objectGroup->name());
    fileDevice->write(objectGroupName.toLatin1().constData());

    fileDevice->write("\n", 1);

    // Write the objects
    for (int i = 0; i < objectGroup->objects().count(); ++i) {
        if (i > 0)
            fileDevice->write("\n", 1);

        const MapObject *object = objectGroup->objectAt(i);
        writeObject(object, fileDevice);
    }
}

void GMapTextPlugin::writeObject(const MapObject *object, QFileDevice *fileDevice)
{
    // Write the object properties
    fileDevice->write("# Object properties\n");
    QString objectName = QString("Name %1\n").arg(object->name());
    fileDevice->write(objectName.toLatin1().constData());

    QString objectType = QString("Type %1\n").arg(object->type());
    fileDevice->write(objectType.toLatin1().constData());

    QString objectPosition =
            QString("Position %1 %2\n")
            .arg(QString::number(object->x()), QString::number(object->y()));
    fileDevice->write(objectPosition.toLatin1().constData());

    QString objectSize =
            QString("Size %1 %2\n")
            .arg(QString::number(object->width()), QString::number(object->height()));
    fileDevice->write(objectSize.toLatin1().constData());

    // TODO Write the custom properties
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
