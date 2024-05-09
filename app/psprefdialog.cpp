#include <QHBoxLayout>
#include <QVBoxLayout>
#include "config.h"
#include "settings.h"
#include "psprefdialog.h"

PsPrefDialog::PsPrefDialog(QWidget* parent) : QDialog (parent)
{
     Settings settings;

    setWindowTitle(tr("Postscript settings"));
    setMinimumSize(400, 100);

    m_mainLayout = new QVBoxLayout;

    /* tunes export */
    QVariant tunes = settings.value(PSTUNES_KEY);
    m_tunesLabel = new QLabel(tr("Tune(s) export"));
    m_tunesComboBox = new QComboBox;
    m_tunesComboBox->setEditable(false);

    QStringList pitems;
    pitems << tr("Selected tune") << tr("All tunes");
    /*
     * 0 = selected tune
     * 1 = all tunes
     */

    m_tunesComboBox->addItems(pitems);
    m_tunesComboBox->setCurrentIndex(tunes.toInt());

    m_tunesLabel->setBuddy(m_tunesComboBox);
    QHBoxLayout* plhbox = new QHBoxLayout;
    plhbox->addWidget(m_tunesLabel);
    plhbox->addWidget(m_tunesComboBox);

    m_mainLayout->addLayout(plhbox);

    /* page number position */
    QVariant pages = settings.value(PSPAGES_KEY);
    m_pagesLabel = new QLabel(tr("Page number position"));
    m_pagesComboBox = new QComboBox;
    m_pagesComboBox->setEditable(false);

    QStringList nitems;
    nitems << tr("no page numbers") << tr("always top left") << tr("always top right") << tr("top left on even pages, else top right") << tr("top right on even pages, else top left");
    /*
     * 0      no page numbers
     * 1      at top left
     * 2      at top right
     * 3      at top left on even pages, top right on odd pages
     * 4      at top right on even pages, top left on odd pages
     */

    m_pagesComboBox->addItems(nitems);
    m_pagesLabel->setBuddy(m_pagesComboBox);
    QHBoxLayout* nphbox = new QHBoxLayout;
    nphbox->addWidget(m_pagesLabel);
    nphbox->addWidget(m_pagesComboBox);

    if (!pages.isNull()) {
        int index = pages.toInt();
        m_pagesComboBox->setCurrentIndex(index);
    }
    m_mainLayout->addLayout(nphbox);

    /* OK / Cancel buttons */
    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    m_mainLayout->addWidget(m_buttons);
    setLayout(m_mainLayout);
}

int PsPrefDialog::getTunesExport()
{
    /*
     * 0 = selected
     * 1 = all
     */
    return m_tunesComboBox->currentIndex();
}

int PsPrefDialog::getPageNumbering()
{
    return m_pagesComboBox->currentIndex();
}
