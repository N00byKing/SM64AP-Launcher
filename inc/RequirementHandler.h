#include <QWidget>
#include <QLineEdit>
#include <QLabel>

class RequirementHandler : public QWidget {
    public:
        static constexpr int window_w = 530;
        static constexpr int window_h = 500;
        RequirementHandler(QWidget* parent, bool advanced);
    private:
        using QWidget::QWidget;
        QLineEdit msys_select{"C:/msys64",this};
        QLabel msys_select_label{"Path to MSYS installation",this};
        void setLocations();
        void setAdvanced(bool);
};
