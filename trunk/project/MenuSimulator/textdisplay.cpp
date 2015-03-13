#include <qpainter.h>
#include <QMouseEvent>
#include "textdisplay.h"

// Constructor
TextDisplay::TextDisplay(QWidget *parent) :
    QFrame(parent)
{
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setLineWidth(1);
    scale = 1.0;
    monoPalette.pixelColor = qRgb(0, 0, 0);
    monoPalette.backgroundColor = qRgb(180, 230, 150);
    image = QImage(width(),height(),QImage::Format_RGB32);
    image.fill(monoPalette.backgroundColor);
    setMinimumSize(QSize(image.width() + lineWidth()*2,image.height() + lineWidth()*2));
    setMouseTracking(1);
    memset(strings, 0, sizeof(strings));
}


// Returns optimal size for widget
QSize TextDisplay::sizeHint(void) const
{
    QSize size = QSize(image.width()*scale+lineWidth()*2,image.height()*scale+lineWidth()*2);
    return size;
}


void TextDisplay::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);
/*    image.fill(monoPalette.backgroundColor);
    QPainter tpainter;
    tpainter.begin(&image);

    QFont font = QFont("Courier New");
    //font.setPixelSize(20);
    font.setPointSize(20);
    //font.setStretch(40);
    //font.setStyleStrategy(QFont::NoAntialias);
    //QFontMetrics fm(font);

    tpainter.setFont(font);
    tpainter.setPen(monoPalette.pixelColor);
    //tpainter.setCompositionMode(QPainter::CompositionMode_Source);
    //QRect br = QRect(0,0,400,50);
    //painter.drawText(x*fm.maxWidth(), y*fm.height(), 300, fm.height(), Qt::AlignLeft | Qt::AlignTop, data, &br);
    for (int i=0; i<4; i++)
    {
        QRect r = QRect(0, i*100, image.width(), 100);
        tpainter.drawText(r, Qt::AlignLeft | Qt::AlignTop, QString::fromLocal8Bit(strings[i]));
    }
    tpainter.end();
*/
    QPainter painter(this);
    //painter.drawImage(lineWidth(),lineWidth(),image.scaled(width()-2*lineWidth(),height()-2*lineWidth()));
    //painter.drawImage(lineWidth(),lineWidth(), image);

    QBrush brush = QBrush(monoPalette.backgroundColor);
    painter.fillRect(lineWidth(),lineWidth(), this->width() - 2*lineWidth(), this->height() - 2*lineWidth(), brush);
    QFont font = QFont("Courier New");
    uint16_t fontHeight = 36;
    font.setPixelSize(fontHeight);
    painter.setFont(font);
    painter.setPen(monoPalette.pixelColor);
    for (int i=0; i<4; i++)
    {
        QRect r = QRect(lineWidth(), lineWidth() + i*(fontHeight+5), width() - lineWidth(), fontHeight);
        painter.drawText(r, Qt::AlignLeft | Qt::AlignTop, QString::fromLocal8Bit(strings[i]));
    }
    displayPixelArea.setCoords( lineWidth(),lineWidth(),width() - lineWidth(), height() - lineWidth() );
}


//---------------------------------------------//
//---------------------------------------------//

// Sets new color for active pixels
void TextDisplay::setMonoPixelColor(const QColor &newColor)
{
    monoPalette.pixelColor = newColor.Rgb;
    update();
}

// Sets new color for background pixels
void TextDisplay::setMonoBackgroundColor(const QColor &newColor)
{
    monoPalette.backgroundColor = newColor.Rgb;
    update();
}

// Sets display width and height
void TextDisplay::setSize(int width, int height)
{
 //   image.~QImage();
 //   image = QImage(width,height,QImage::Format_RGB32);
 //   image.fill(monoPalette.backgroundColor);
    setMinimumSize(QSize(image.width() + lineWidth()*2,image.height() + lineWidth()*2));
    updateGeometry();
}

void TextDisplay::setScale(float newScale)
{
    scale = newScale;
    updateGeometry();
}


void TextDisplay::putStringXY(int x, int y, const char *data)
{
    strcpy(&strings[y][x], data);
    this->update();
}

void TextDisplay::updateAll(char **data)
{
    strcpy(strings[0], data[0]);
    strcpy(strings[1], data[1]);
    strcpy(strings[2], data[2]);
    strcpy(strings[3], data[3]);
    this->update();
}





