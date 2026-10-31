#include "Theme.h"

QString Theme::styleSheet(bool dark) {
    if (!dark) return QString();  // default Qt light look
    return R"(
      QWidget { background: #14161a; color: #e6e8eb; font-size: 13px; }
      QListWidget { background: #1b1e24; border: none; padding: 6px; }
      QListWidget::item { padding: 9px 12px; border-radius: 6px; }
      QListWidget::item:selected { background: #2f6fed; color: white; }
      QListWidget::item:hover:!selected { background: #262a32; }
      QPushButton { background: #262a32; border: 1px solid #343944;
                    padding: 6px 14px; border-radius: 6px; }
      QPushButton:hover { background: #303540; }
      QStatusBar { background: #1b1e24; }
    )";
}
