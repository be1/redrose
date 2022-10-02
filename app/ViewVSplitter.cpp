#include "ViewVSplitter.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPainter>
#include <QSvgRenderer>

ViewVSplitter::ViewVSplitter(QWidget* parent)
    : QSplitter(parent)
    , prev(QIcon::fromTheme("previous"), tr("Previous"), this)
    , print(QIcon::fromTheme("document-print"), tr("Print"), this)
    , next(QIcon::fromTheme("next"), tr("Next"), this)
{
    setOrientation(Qt::Vertical);

    /* configure the svg score */
    QPalette p = svgwidget.palette();
    p.setColor(svgwidget.backgroundRole(), Qt::white);
    svgwidget.setPalette(p);
    svgwidget.setAutoFillBackground(true);
    svgwidget.setMinimumSize(630, 891);

    /* configure the scroll area */
    area = new QScrollArea();
    area->setWidget(&svgwidget);

    /* configure the pages buttons */
    QWidget *pagesWidget = new QWidget(this);
    pagesWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    connect(&prev, &QPushButton::clicked, this, &ViewVSplitter::prevPageClicked);
    pageslayout.addWidget(&prev, 0, Qt::AlignLeft);
    connect(&print, &QPushButton::clicked, this, &ViewVSplitter::printClicked);
    pageslayout.addWidget(&print, 0, Qt::AlignCenter);
    connect(&next, &QPushButton::clicked, this, &ViewVSplitter::nextPageClicked);
    pageslayout.addWidget(&next, 0, Qt::AlignRight);
    pagesWidget->setLayout(&pageslayout);
    pagesWidget->setFixedHeight(pageslayout.sizeHint().height());

    /* add all in good order to the splitter */
    addWidget(area);
    addWidget(pagesWidget);
    addWidget(&logview);
    QList<int> sizes;
    sizes.append(594);
    sizes.append(100);
    sizes.append(0);
    setSizes(sizes);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setCollapsible(2, true);
}

ViewVSplitter::~ViewVSplitter()
{
}

LogView *ViewVSplitter::logView()
{
    return &logview;
}

void ViewVSplitter::initBasename(const QString &b, const QString &d)
{
    qDebug() << "check" << b;
    basename = b;
    basedir = d;
    currentpage = 0;
    for (int i = 1; i <= 999; i++) {
        QString nnn = QString::asprintf("%03d", i);
        QString temp(d + QDir::separator() + b);
        temp += nnn;
        temp += ".svg";
        if (!QFile::exists(temp)) {
            lastpage = i - 1;
            break;
        }
        svgnames.append(temp);
    }
}

bool ViewVSplitter::requestPage(int i) {
    qDebug() << "requesting page" << i;
    int page = i + currentpage;
    if (page > 0 && page <= lastpage) {
        currentpage = page;
        QString nnn = QString::asprintf("%03d", page);
        QString temp(basedir + QDir::separator() + basename);
        temp += nnn;
        temp += ".svg";
        svgWidget()->load(temp);

        if (page > 1)
            prev.setEnabled(true);
        else
            prev.setEnabled(false);

        if (page < lastpage)
            next.setEnabled(true);
        else
            next.setEnabled(false);
        return true;
    }

    /* else, empty view */
    svgWidget()->load(QString());
    prev.setEnabled(false);
    next.setEnabled(false);
    return false;
}

void ViewVSplitter::cleanup()
{
    svgnames.clear();
    initBasename(QString(), QString());
}

ScoreSvgWidget *ViewVSplitter::svgWidget()
{
    return &svgwidget;
}

void ViewVSplitter::prevPageClicked()
{
    requestPage(-1);
}

void ViewVSplitter::printClicked()
{
    QPrinter printer;
    printer.setCreator("Redrose");
    printer.setDocName("abc_score");
    printer.setPageOrientation(QPageLayout::Portrait);
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        if (printer.isValid()) {
            QPainter painter;
            if (!painter.begin(&printer)) {
                qWarning() << "Could not start printing";
                return;
            }

            requestPage(-currentpage +1);
            for (int i = 0; i < svgnames.length(); i++) {
                svgwidget.renderer()->render(&painter);
                if (i < svgnames.length() - 1) {
                    requestPage(1);
                    printer.newPage();
                }
            }
            painter.end();
            requestPage(-currentpage +1);
        }
    }
}


void ViewVSplitter::nextPageClicked()
{
    requestPage(1);
}
