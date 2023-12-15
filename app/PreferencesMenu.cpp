#include "PreferencesMenu.h"
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "AbcApplication.h"
#include "editorprefdialog.h"
#include "playerprefdialog.h"
#include "settings.h"
#include "config.h"

PreferencesMenu::PreferencesMenu(QWidget* parent)
    : QMenu(parent)
{
    setTitle(tr("Preferences"));

    addAction(tr("Player settings"), this, &PreferencesMenu::onPlayerActionTriggered);
    addAction(tr("Postscript export"), this, &PreferencesMenu::onPsActionTriggered);
    addAction(tr("Editor settings"), this, &PreferencesMenu::onEditorActionTriggered);
    addAction(tr("Reset settings"), this, &PreferencesMenu::onResetActionTriggered);
}

PreferencesMenu::~PreferencesMenu()
{
}

void PreferencesMenu::onPlayerActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    PlayerPrefDialog* dialog = new PlayerPrefDialog(a->mainWindow());

    if (QDialog::Accepted == dialog->exec()) {
        Settings settings;

        settings.setValue(PLAYER_KEY, dialog->getPlayer());
        settings.setValue(DRIVER_KEY, dialog->getDriver());
        settings.setValue(SOUNDFONT_KEY, dialog->getSoundfont());
        settings.setValue(PLAYER_DURATION, dialog->getDuration());
        settings.setValue(PLAYER_VELOCITY, dialog->getVelocity());
#if 0
        settings.setValue(REVERB_KEY, dialog->getReverb());
#endif
        settings.sync();
    }

    delete dialog;
}

void PreferencesMenu::onPsActionTriggered()
{
    Settings settings;
    QVariant ps = settings.value(PSTUNES_KEY);

    AbcApplication* a = static_cast<AbcApplication*>(qApp);

    QStringList items;
    if (!ps.isNull()) {
        items << ps.toString();
    }

    items << TUNES_SELECTED << TUNES_ALL;
    items.removeDuplicates();

    bool ok;
    QString param;
    param = QInputDialog::getItem(a->mainWindow(), tr("Postscript export preference"), tr("Tunes:"), items, 0, false, &ok);

    if (!ok)
        return;

    settings.setValue(PSTUNES_KEY, param);
    settings.sync();

}

void PreferencesMenu::onResetActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);

    if (QMessageBox::No == QMessageBox::question(a->mainWindow(), tr("Reset prefrences?"), tr("Do you really want to reset preferences?")))
        return;

    Settings settings;
    settings.reset();
}

void PreferencesMenu::onEditorActionTriggered()
{
    AbcApplication* a = static_cast<AbcApplication*>(qApp);
    EditorPrefDialog* dialog = new EditorPrefDialog(a->mainWindow());

    if (QDialog::Accepted == dialog->exec()) {
        Settings settings;

        settings.setValue(EDITOR_FONT_BASE, dialog->getBaseFont().family());
        settings.setValue(EDITOR_FONT_RANGE, dialog->getFontRange());
        settings.setValue(EDITOR_HIGHLIGHT, dialog->getHighlight());
        settings.setValue(EDITOR_AUTOPLAY, dialog->getAutoplay());
        settings.setValue(EDITOR_BAR_COLOR, dialog->getColor(EDITOR_BAR_COLOR));
        settings.setValue(EDITOR_COMMENT_COLOR, dialog->getColor(EDITOR_COMMENT_COLOR));
        settings.setValue(EDITOR_DECORATION_COLOR, dialog->getColor(EDITOR_DECORATION_COLOR));
        settings.setValue(EDITOR_EXTRAINSTR_COLOR, dialog->getColor(EDITOR_EXTRAINSTR_COLOR));
        settings.setValue(EDITOR_GCHORD_COLOR, dialog->getColor(EDITOR_GCHORD_COLOR));
        settings.setValue(EDITOR_HEADER_COLOR, dialog->getColor(EDITOR_HEADER_COLOR));
        settings.setValue(EDITOR_LYRIC_COLOR, dialog->getColor(EDITOR_LYRIC_COLOR));
        settings.sync();
    }

    delete dialog;
}
