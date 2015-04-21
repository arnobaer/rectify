/*
 *  rectify - Orthophoto rectification for archaeological use.
 *  Copyright (C) 2015  Bernhard Arnold
 *                2004  Marcelo Teixeira Silveira, Rafael Paz,
 *                      Orlando Bernardo Filho, Sidney Andrade de Lima,
 *                      Luiz Coelho
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see {http://www.gnu.org/licenses/}.
 *
 */

#ifndef Rectifier_h
#define Rectifier_h

#include "Matrix.h"

#include <QtGui/QColor>
#include <QObject>

class Image;

class Rectifier : public QObject
{
    Q_OBJECT

public:
    Rectifier(Image *o, Image *r, int intp);

    Image *original, *retificada;
    int interpolacao;

public:
    void afimGeral(int modo);
    void afimIsogonal();
    void linearDireta();
    void projetiva();
    QRgb achaCor(float x, float y);
    float df(float x);
    int acertaPixel(int cor);
    void report(const Matrix& m);
    void report(const QString& message);


signals:
    void report_(const QString& message);

protected:

private:
};

#endif // Rectifier_h
