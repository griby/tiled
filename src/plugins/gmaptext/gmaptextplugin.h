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

#pragma once

#include "gmaptext_global.h"

#include "mapformat.h"
#include "tilelayer.h"
#include "objectgroup.h"

#include "savefile.h"

namespace GMapText {

class GMAPTEXTSHARED_EXPORT GMapTextPlugin : public Tiled::WritableMapFormat
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.mapeditor.MapFormat" FILE "plugin.json")

public:
    GMapTextPlugin();

    bool write(const Tiled::Map *map, const QString &fileName) override;

    QString nameFilter() const override;
    QString shortName() const override;
    QString errorString() const override;

private:
    void writeMap(const Tiled::Map *map, QFileDevice *fileDevice);
    void writeTileLayer(const Tiled::TileLayer *tileLayer, QFileDevice *fileDevice);
    void writeObjectGroup(const Tiled::ObjectGroup *objectGroup, QFileDevice *fileDevice);
    void writeObject(const Tiled::MapObject *object, QFileDevice *fileDevice);

    QString mError;
};

} // namespace GMapText
