
#
#ifndef	__DONGLE_SELECT
#define	__DONGLE_SELECT
#
#include	<QDialog>
#include	<QLabel>
#include	<QListView>
#include	<QStringListModel>
#include	<QStringList>
#include	<stdint.h>

class	dongleSelect: public QDialog {
Q_OBJECT
public:
	dongleSelect	(void);
	~dongleSelect	(void);
void	addtoDongleList	(const char *);
int16_t	getSelectedItem	(void);
private:
	QLabel		*toptext;
	QListView	*selectorDisplay;
	QStringListModel	dongleList;
	QStringList	Dongles;
	int16_t		selectedItem;
private slots:
void	selectDongle	(QModelIndex);
};

#endif

