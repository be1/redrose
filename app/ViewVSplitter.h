#ifndef VIEWVBOXLAYOUT_H
#define VIEWVBOXLAYOUT_H

#include "scorepswidget.h"
#include <QSplitter>
#include <QHBoxLayout>
#include <QPushButton>

class ViewVSplitter: public QSplitter
{
	Q_OBJECT

public:
    ViewVSplitter(QWidget* parent = nullptr);
    ~ViewVSplitter();

    ScorePsWidget* psWidget();
    QPushButton* prevButton() { return &prev; }
    QPushButton* printButton() { return &print; }
    QPushButton* nextButton() { return &next; }

    void initBasename(const QString& orig, const QString& tmpbase, const QString& tmpdir);

    long currentPage() { return currentpage; }
    long lastPage() { return lastpage; }

    bool gotoPage(int page); /* starts with 1! 0 means cleanup */
    bool turnPage(int diff);

    void cleanup();

protected:
    void resizeEvent(QResizeEvent *event) override;

protected slots:
    void prevPageClicked();
    void printClicked();
    void nextPageClicked();

private:
    ScorePsWidget pswidget;
    QWidget pagesWidget;
    QHBoxLayout pageslayout;
    QPushButton prev;
    QPushButton print;
    QPushButton next;

    long currentpage = 0;
    long lastpage = 0;
    QString origname;
    QString basename;
    QString basedir;
};
#endif
