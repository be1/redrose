#include "ViewVSplitter.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPainter>
#include <QScrollBar>

ViewVSplitter::ViewVSplitter(QWidget* parent)
    : QSplitter(parent)
    , prev(QIcon::fromTheme("go-previous"), tr("Previous"), this)
    , print(QIcon::fromTheme("document-print"), tr("Print"), this)
    , next(QIcon::fromTheme("go-next"), tr("Next"), this)
{
    setOrientation(Qt::Vertical);

    /* configure the ps score */
    QPalette p = pswidget.palette();
    p.setColor(pswidget.backgroundRole(), Qt::white);
    pswidget.setPalette(p);
    pswidget.setAutoFillBackground(true);
    pswidget.setMinimumHeight(297);

    /* configure the scroll area */
    pswidget.setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    pswidget.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    /* configure the pages buttons */
    pagesWidget.setMinimumHeight(210);
    pagesWidget.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    connect(&prev, &QPushButton::clicked, this, &ViewVSplitter::prevPageClicked);
    pageslayout.addWidget(&prev, 0, Qt::AlignLeft);
    connect(&print, &QPushButton::clicked, this, &ViewVSplitter::printClicked);
    pageslayout.addWidget(&print, 0, Qt::AlignCenter);
    connect(&next, &QPushButton::clicked, this, &ViewVSplitter::nextPageClicked);
    pageslayout.addWidget(&next, 0, Qt::AlignRight);
    pagesWidget.setLayout(&pageslayout);
    pagesWidget.setFixedHeight(pageslayout.sizeHint().height());

    /* add all in good order to the splitter */
    addWidget(&pswidget);
    addWidget(&pagesWidget);
    QList<int> sizes;
    sizes.append(594);
    sizes.append(100);
    setSizes(sizes);
    setCollapsible(0, false);
    setCollapsible(1, false);

    prev.setEnabled(false);
    print.setEnabled(false);
    next.setEnabled(false);
}

ViewVSplitter::~ViewVSplitter()
{
}

void ViewVSplitter::initBasename(const QString &orig, const QString &tmpbase, const QString &tmpdir)
{
    qDebug() << __func__ << tmpbase;
    origname = orig;
    basename = tmpbase;
    basedir = tmpdir;
    currentpage = 0; /* invalidate */
    psWidget()->load(tmpdir + QDir::separator() + tmpbase + ".ps");
    lastpage = psWidget()->getNumberOfPages();
}

bool ViewVSplitter::gotoPage (int page) {
    if (page > 0 && page <= lastpage) {
        currentpage = page;
        psWidget()->displayPage(page -1);
        print.setEnabled(true);

        if (page > 1)
            prev.setEnabled(true);
        else
            prev.setEnabled(false);

        if (page < lastpage)
            next.setEnabled(true);
        else
            next.setEnabled(false);

        pswidget.verticalScrollBar()->setSliderPosition(0);
        return true;
    }

    /* else, empty view */
    cleanup();
    return false;
}

bool ViewVSplitter::turnPage(int diff) {
    int page = diff + currentpage;
    qDebug() << __func__ << page;

    return gotoPage(page);
}

void ViewVSplitter::cleanup()
{
    pswidget.displayPage(-1);
    prev.setEnabled(false);
    print.setEnabled(false);
    next.setEnabled(false);
    pswidget.verticalScrollBar()->setSliderPosition(0);
}

void ViewVSplitter::resizeEvent(QResizeEvent *event) {
    QSplitter::resizeEvent(event);

    int w, h;
    int viewportWidth = pswidget.scene()->width();
    int viewportHeight = pswidget.scene()->height();

    //viewportWidth = viewportWidth > 210 ? viewportWidth : 210;
    if (viewportWidth * 297 > viewportHeight * 210) {
        w = viewportWidth;
        h = viewportWidth * 297 / 210;
    } else {
        w = viewportHeight * 210 / 297;
        h = viewportHeight;
    }

    pswidget.viewport()->resize(w, h);
}

ScorePsWidget *ViewVSplitter::psWidget()
{
    return &pswidget;
}

void ViewVSplitter::prevPageClicked()
{
    turnPage(-1);
}

void ViewVSplitter::printClicked()
{
    QPrinter printer(QPrinter::PrinterResolution);
    /* FIXME: HighResolution (1200 DPI) freeze main thread */
    printer.setResolution(300.); /* 300 DPI is enough */
    printer.setCreator("Redrose");
    printer.setDocName(origname);
    QFileInfo info(origname);
    printer.setOutputFileName(info.absolutePath() + QDir::separator() + info.baseName() + ".pdf");
    printer.setPageOrientation(QPageLayout::Portrait);
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        if (printer.isValid()) {
            QPainter painter;
            if (!painter.begin(&printer)) {
                qWarning() << "Could not start printing";
                return;
            }

            QPrinter::PrintRange range = printer.printRange();
            QList<int> pages;
            switch (range) {
            case QPrinter::AllPages:
                for (int i = 0; i < lastpage; i++) {
                    pages.append(i);
                }
                break;
            case QPrinter::Selection:
                /* FIXME */
                qDebug() << printer.printerSelectionOption();
                break;
            case QPrinter::PageRange:
                for (int i = printer.fromPage() -1; i < printer.toPage(); i++) {
                    pages.append(i);
                }
                break;
            case QPrinter::CurrentPage:
                pages.append(currentpage -1);
                break;
            default:
                break;
            }

            for (auto p : pages) {
                pswidget.printPage(p, &painter);
                if (p < pages.size() - 1) {
                    printer.newPage();
                }
            }
            painter.end();
        }
    }
}


void ViewVSplitter::nextPageClicked()
{
    turnPage(1);
}
