#ifndef PIXELDISPLAY_H
#define PIXELDISPLAY_H

#include <QWidget>
#include <QFrame>
#include <qimage.h>
#include <qwidget.h>







class TextDisplay : public QFrame
{
    Q_OBJECT
public:
    explicit TextDisplay(QWidget *parent = 0);

    typedef struct {
        QRgb pixelColor;
        QRgb backgroundColor;
    } monochrome_palette_t;

    monochrome_palette_t monoPalette;
    char strings[4][40];

    void setMonoPixelColor(const QColor &newColor);
    void setMonoBackgroundColor(const QColor &newColor);
    void setScale(float newScale);
    void setSize(int width, int height);
    void setStringCount(int stringLength, int numOfStrings);
    void putStringXY(int x, int y, const char *data);
    void updateAll(char **data);
signals:

public slots:


protected:
    void paintEvent(QPaintEvent *event);
    QSize sizeHint(void) const;
private:
    QImage image;
    float scale;
    QRect displayPixelArea;
    void updateFrom_MONOCHROME_3310_8bit(uint8_t *buffer);
    void updateFrom_RGB_888_32bit(uint32_t *buffer);
};

#endif // PIXELDISPLAY_H
