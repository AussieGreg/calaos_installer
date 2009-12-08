#ifndef DIALOGNEWTEMP_H
#define DIALOGNEWTEMP_H

#include <QtGui/QDialog>
#include <iostream>

#include <ListeRoom.h>
using namespace std;
using namespace Calaos;

namespace Ui
{
        class DialogNewTemp;
}

class DialogNewTemp : public QDialog
{
                Q_OBJECT
                Q_DISABLE_COPY(DialogNewTemp)
        public:
                explicit DialogNewTemp(Room *room, QWidget *parent = 0);
                virtual ~DialogNewTemp();

                Input *getInput() { return temperature; }

        protected:
                virtual void changeEvent(QEvent *e);

        private:
                Ui::DialogNewTemp *ui;
                Input *temperature;
                Room *room;

        private slots:
                void on_buttonBox_accepted();
};

#endif // DIALOGNEWTEMP_H
