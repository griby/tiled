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

#include "map.h"
#include "savefile.h"
#include "tile.h"
#include "tilelayer.h"

#include <QDir>
#include <QFileInfo>

using namespace Tiled;
using namespace GMapText;

GMapTextPlugin::GMapTextPlugin()
{
}

bool GMapTextPlugin::write(const Map *map, const QString &fileName)
{
    SaveFile file(fileName);

    // Open the exported file
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        mError = tr("Could not open file for writing.");
        return false;
    }

    auto device = file.device();

    // Go through all the tile layers
    uint currentLayer = 1;
    for (const Layer *layer : map->layers()) {

        // Ignore other layer types
        if (layer->layerType() != Layer::TileLayerType)
            continue;

        const TileLayer *tileLayer = static_cast<const TileLayer*>(layer);

        // Write out the layer ID
        QString layerName = QString("Layer #%1\n").arg(currentLayer);
        device->write(layerName.toLatin1().constData());

        // Write out tiles either by ID or their name, if given. -1 is "empty"
        for (int y = 0; y < tileLayer->height(); ++y) {
            for (int x = 0; x < tileLayer->width(); ++x) {

                // Separate tile IDs with a single space
                if (x > 0)
                    device->write(" ", 1);
    
                const Cell &cell = tileLayer->cellAt(x, y);
                const Tile *tile = cell.tile();
                const int id = tile ? tile->id() : -1;
                device->write(QByteArray::number(id));
            }

            device->write("\n", 1);
        }

        device->write("\n", 1);

        ++currentLayer;
    }

    if (file.error() != QFileDevice::NoError) {
        mError = file.errorString();
        return false;
    }

    if (!file.commit()) {
        mError = file.errorString();
        return false;
    }

    return true;
}

QString GMapTextPlugin::nameFilter() const
{
    return tr("Griby map text files (*.gmt)");
}

QString GMapTextPlugin::shortName() const
{
    return QLatin1String("gmaptext");
}

QString GMapTextPlugin::errorString() const
{
    return mError;
}
