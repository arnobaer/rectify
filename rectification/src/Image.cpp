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

#include "Image.h"
#include "MainWindow.h"

#include <QtGui/QImage>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QMessageBox>

#include <cstdio>

extern MainWindow *mainWindow;

Image::Image(int orids)
{
    // Construtor da classe imagem
    setGeometry(0, 0, 1, 1);
//     setBackgroundMode(Qt::NoBackground);
    setMouseTracking(1);
    zeroPontos(0);
    setCursor(Qt::CrossCursor);
    orides = orids;
    updateCursor(Qt::yellow, QSize(19, 19));
}

void Image::updateCursor(const QColor& color, const QSize size)
{
    // Create custom crosshair cursor.
    QPixmap pix(size);
    pix.fill(Qt::transparent);
    QPainter paint;
    paint.begin(&pix);
    paint.setPen(QPen(color));
    paint.drawLine(pix.width() / 2, 0, pix.width() / 2, pix.height());
    paint.drawLine(0, pix.height() / 2, pix.width(), pix.height() / 2);
    paint.end();
    setCursor(QCursor(pix));
}

void Image::zeroPontos(int inicio)
{
    for (int i = inicio; i < 20; i++)
    {
        pontos[i][0] = 0;
        pontos[i][1] = 0;
        pontos[i][2] = 0;
    }
}

void Image::openImage(QString arquivo)
{
    if (!figura.load(arquivo))
        printf("Image could not be loaded!\n");
    this->setGeometry(0, 0, figura.width(), figura.height());
//     fixImageDepth();
//     pix->convertFromImage(*figura, QPixmap::Auto );
}

void Image::fixImageDepth()
{
//     if (figura.depth() < 32)
//     {
// //         QImage *oldPicture;
// //         oldPicture = figura;
// //         *figura = figura->convertDepth(32, Qt::AutoColor);
// //		delete oldPicture;
//     }
}

void Image::saveImage(QString arquivo)
{
    if (!figura.save(arquivo, "PNG"))
        printf("Error while saving!\n");
}

void Image::paintEvent(QPaintEvent*)
{
    // Desenha as marcas
    QPixmap pix(figura.size());
    pix = QPixmap::fromImage(figura);
    if (!figura.isNull())
    {
        QPainter paint;
        QBrush brush( mainWindow->cursorColor, Qt::NoBrush );
        QPen yellow(mainWindow->cursorColor, 1, Qt::SolidLine);
        QPen red(Qt::red, 1, Qt::SolidLine);
        paint.begin(&pix);
        paint.setBrush( brush );
        int i = 0, j;
        // Conta o total de pontos selecionados
        while ((i < 21) && (pontos[i][2]))
            i++;
        //  Desenha o retângulo de ajuda proporcinal
        if ((i >= 2) && (mainWindow->checkBox1->isChecked()) && (orides))
        {
            paint.setPen(red);
            int vr_width = abs(pontos[0][0] - pontos[1][0]) + 1;
            int vr_height = vr_width * (1 / mainWindow->ratioLabel->text().toDouble()) + 1;
            if (abs(pontos[0][0] - pontos[1][0]) < abs(pontos[0][1] - pontos[1][1]))
            {
                vr_height = abs(pontos[0][1] - pontos[1][1]) + 1;
                vr_width = vr_height * (mainWindow->ratioLabel->text().toDouble()) + 1;
            }
            int vr_x = pontos[0][0];
            int vr_y = pontos[0][1];
            if (pontos[1][0] < vr_x)
                vr_x = pontos[1][0];
            if (pontos[1][1] < vr_y)
                vr_y = pontos[1][1];
            if ((vr_x) > figura.width() / 2)
                vr_width = vr_width * -1;
            if ((vr_y) > figura.height() / 2)
                vr_height = vr_height * -1;
            paint.drawRect(vr_x, vr_y, vr_width, vr_height);
        }
        // Desenha os pontos na tela
        paint.setPen(yellow);
        for (j = 0; j < i; j++)
        {
            paint.drawPoint(pontos[j][0], pontos[j][1]);
            paint.drawEllipse(pontos[j][0] - 6, pontos[j][1] - 6, 12, 12);
            paint.drawText(QPoint(pontos[j][0] - 2, pontos[j][1] + 20), QString::number(j + 1));
            // Liga os pontos, se selecionado
            if ((j > 0) && (mainWindow->spinReturn(3)))
                paint.drawLine(pontos[j - 1][0], pontos[j - 1][1], pontos[j][0], pontos[j][1]);
        }
        // Liga os últimos pontos
        if ((i > 2) && (mainWindow->spinReturn(3)))
            paint.drawLine(pontos[i - 1][0], pontos[i - 1][1], pontos[0][0], pontos[0][1]);
        paint.end();
    }

//     bitBlt(this, 0, 0, &pix);
    QPainter p(this);
    p.drawPixmap(0, 0, pix.width(), pix.height(), pix);
}

void Image::mouseMoveEvent(QMouseEvent *e)
{
// Informa as coordenadas
    if ((e->x() >= 0) && (e->y() >= 0) && (e->x() < figura.width()) && (e->y() < figura.height()))
    {
        mainWindow->coluna->setText(QString::number(e->x()));
        mainWindow->linha->setText(QString::number(e->y()));
    }
    drawZoom(e->x(), e->y());
}

void Image::drawZoom(int x, int y)
{
// Desenha o zoom
    QPixmap pix = QPixmap(20, 20);
    pix.fill(Qt::transparent);
//bitBlt(pix,0,0,this,x-10,y-10,20,20);
    QPainter paint;
    QBrush bgbrush( QColor("#333"), Qt::SolidPattern );
    paint.begin(&pix);
// Image
    paint.drawImage(0, 0, figura, x - 10, y - 10, 20, 20);
// Bordas
    if (x - 10 < 0)
        paint.fillRect(0, 0, 10 - x, 20, bgbrush);
    if (y - 10 < 0)
        paint.fillRect(0, 0, 20, 10 - y, bgbrush);
    if (x + 9 > figura.width() - 1)
        paint.fillRect(20, 0, (figura.width() - 1) - x - 9, 20, bgbrush);
    if (y + 9 > figura.height() - 1)
        paint.fillRect(0, 20, 20, (figura.height() - 1) - y - 9, bgbrush);
    paint.end();
// Copia do buffer para o Label
    QPixmap zoom = pix.scaledToWidth(100);
    paint.begin(&zoom);
    paint.setPen(QPen(mainWindow->cursorColor));
    paint.drawLine(zoom.width() / 2, 0, zoom.width() / 2, zoom.height());
    paint.drawLine(0, zoom.height() / 2, zoom.width(), zoom.height() / 2);
    paint.end();
    mainWindow->zoomLabel->setPixmap(zoom);
}

void Image::mousePressEvent(QMouseEvent *e)
{
    int pt = mainWindow->spinReturn(0) - 1;
    if ((pt > 0) && (!pontos[pt - 1][2]))
    {
        QMessageBox::warning( this, "Warning:",
                              "You must select points in sequence !\n\n ",
                              "Ok",
                              0);
        return;
    }
    pontos[pt][0] = e->x();
    pontos[pt][1] = e->y();
    pontos[pt][2] = 1;
    repaint();
    drawZoom(e->x(), e->y());
    mainWindow->recebePontos(e->x(), e->y());
}

void Image::imageInfo(int enable)
{
    // Largura, altura, enabled
    mainWindow->dadosImage(figura.width(), figura.height(), enable);
    mainWindow->varLarguraLabel->setText("(0-" + QString::number(figura.width() - 1) + ")");
    mainWindow->varAlturaLabel->setText("(0-" + QString::number(figura.height() - 1) + ")");
}

void Image::report(const QString& message)
{
    emit report_(message);
}
