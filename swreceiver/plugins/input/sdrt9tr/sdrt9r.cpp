#
/*
 *    Copyright (C) 2014: Jan van Katwijk & Johan Maas PA3GSB
 *
 *
 *
 *
 *
 *     Communication via network.
 *
 *     - Using broadcast to initialize the SDR hardware
 *     - Receiving broadcast for checking if the SDR is initialized.
 *     - After starting the receiver the data is read using UDP.
 *     - Via UDP datagrams the SDR is controlled (setting frequency and data rate)
 *
 */


#include	<QSettings>
#include	<QLabel>
#include	<QMessageBox>
#include	<QLineEdit>
#include	<QUdpSocket>
#include	<QHostAddress>
#include	"sdrt9r.h"
//
#define	DEFAULT_FREQUENCY	(Khz (14070))


QWidget	*sdrt9r::createPluginWindow (int32_t rate, QSettings *s) {

	(void)rate;		// not used here (starting at 6.0)
	sdrt9rSettings		= s;
	theFrame		= new QFrame;
	setupUi (theFrame);

    //	setting the defaults and constants
	vfoOffset	= 0;
	theRate		= sdrt9rSettings ->
	                          value ("sdrt9r-rate", 96000). toInt ();
	rateDisplay	-> display (theRate);
	Normalizer	= 8388608.0;	// 2 ^ 24
	dataAllowed	= false;
	sdrFound = false;
	vfoFrequency	= DEFAULT_FREQUENCY;
	_I_Buffer	= new RingBuffer<int32_t>(2 * 32768);
	theLineEditor	= NULL;
	theSocket	= NULL;
	connected	= false;
	connect (hzOffset, SIGNAL (valueChanged (int)),
	         this, SLOT (setOffset (int)));
	connect (connectButton, SIGNAL (clicked (void)),
	         this, SLOT (wantConnect (void)));
	connect (rateSelector, SIGNAL (activated (const QString &)),
	         this, SLOT (setRate (const QString &)));
	state	-> setText ("waiting to start");
	return theFrame;
}

//
//	just a dummy, needed since the rigInterface is abstract
//	and we need to terminate properly
	rigInterface::~rigInterface	(void) {
}


	sdrt9r::~sdrt9r	(void) {
	if (connected) {		// close previous connection
	   theSocket	-> close ();
	   delete theSocket;
	   sdrt9rSettings -> setValue ("sdrt9r-rate", theRate);
	}
	delete	_I_Buffer;
}

//
//	After creation of the device-window, the user may press
//	a button to try to set up a connection.
//	Pressing the button activated the function "wantConnect"
//	After pressing the button any previous connection
//	is terminated, and a window appears for giving the
//	IP address
void	sdrt9r::wantConnect (void) {
	if (connected) {
	   theSocket	-> close ();
	   delete	theSocket;
	   theSocket	= NULL;
	   connected	= false;
	}

	theLineEditor	= new QLineEdit (NULL);
	theLineEditor	-> setInputMask ("000.000.000.000");
//	Setting default IP address
	theLineEditor -> setText("169254190180");
	theLineEditor	-> show ();
	state	-> setText ("Give IP address, return");
	connect (theLineEditor, SIGNAL (returnPressed (void)),
	         this, SLOT (setConnection (void)));
}

//	if/when a return is pressed in the line edit,
//	a signal appears and we are able to collect the
//	inserted text. The format is the IP-V4 format.
//	Using this text, we try to connect,
void	sdrt9r::setConnection (void) {
QString s = theLineEditor -> text ();
QHostAddress theAddress	= QHostAddress (s);

//	setting broadcast (to initialize SDR) and Port
	udpSocketBroadcasrTx = new QUdpSocket (this);
	udpSocketBroadcasrTx -> bind (QHostAddress ("169.254.190.180"), 20070); 

//	setting to read broadcast from SDR
	udpSocketBroadcastRx = new QUdpSocket(this);
	if (!udpSocketBroadcastRx -> bind (QHostAddress::Broadcast, 20070)) {
	   fprintf (stderr, "connection failed\n");
	   return;	
	}
	int noRetries = 0;
//	Lets try it max 15 times; why 15... it a nice number!
	while (!sdrFound && noRetries < 15){
	   broadcastDatagram ();
	   readBroadcastDatagram ();
	   noRetries ++;
	}

//	Releasing the sockets.
	delete udpSocketBroadcasrTx;
	delete udpSocketBroadcastRx;
	udpSocketBroadcasrTx	= NULL;	 // !connected <=> udpSocket == NULL
	udpSocketBroadcastRx = NULL;
	delete theLineEditor;

	if (!sdrFound) {
	   state -> setText ("SDR Connection failed");
	   QMessageBox::warning (theFrame,
	                         tr("sdrt9r"),
	                          tr("SDR Connection Failed"));
        return;
    }


//	Initialize the receiving socket.
	theSocket	= new QUdpSocket (this);
	if (!theSocket -> bind (theAddress, 20010)) {
	   state -> setText ("Local Socket Connection failed");
	   QMessageBox::warning (theFrame,
                             tr("sdrt9r"), tr("Local Socket Connection Failed"));
	   delete theSocket;
	   theSocket	= NULL;		// !connected <=> theSocket == NULL
	   return;
	}

//
//	Do whatever communication is needed here,
//	If anything is wrong, just copy the lines from above
//	after the connect, data from the device will be read
//	by the function "readData"
	connect (theSocket, SIGNAL (readyRead (void)),
	         this, SLOT (readData (void)));
	state -> setText ("connected");
	connected = true;
}

int32_t	sdrt9r::getRate	(void) {
//	QString value = QString::number (theRate);
//	qDebug() << "Get Rate = " + value;

	return theRate;
}

void	sdrt9r::setRate	(const QString &s) {
int32_t	localRate	= s. toInt ();

//    QString value = QString::number(localRate);
//    qDebug() << "Set Rate = " + value;

	if (localRate == theRate)
	   return;

//	communicate change in rate to the device
	theRate	= localRate;
	rateDisplay -> display (theRate);
    	set_changeRate (theRate);	// signal the main program
	                                // to alter some settings

        if (connected)
	   sendControlDatagram();
}

bool	sdrt9r::legalFrequency (int32_t f) {
	return Khz (500) <= f && f <= Mhz (55);
}

int32_t	sdrt9r::defaultFrequency	(void) {
	return DEFAULT_FREQUENCY;	// choose any legal frequency here
}

void	sdrt9r::setVFOFrequency	(int32_t newFrequency) {
	if (!connected)
	   return;

//	here the command to set the frequency
	vfoFrequency = newFrequency;
//	QString value = QString::number(vfoFrequency);
//	qDebug() << "Set VFO freq = " + value;

//	tell the SDR hw the new frequency
	sendControlDatagram();
}

int32_t	sdrt9r::getVFOFrequency	(void) {
	return vfoFrequency - vfoOffset;
}

bool	sdrt9r::restartReader	(void) {
	dataAllowed	= true;
//	qDebug() << "Start" ;
	return true;
}

void	sdrt9r::stopReader	(void) {
	dataAllowed	= false;
//    qDebug() << "Stop" ;
}

void	sdrt9r::exit		(void) {
	stopReader ();
}

//
//	Note that normal use in the radio software is to first
//	enquire the number of samples available
//	So, normally, the "size" samples requested are available
int32_t	sdrt9r::getSamples (DSPCOMPLEX *V, int32_t size, uint8_t Mode) { 
int32_t buf [2 * size];
int32_t	amount;
int32_t	i;

	if (!connected) {
//	qDebug() << "not connected ";
	   return size;
	}

//	Note that we need two int32_t's to build one sample
	amount = _I_Buffer	-> getDataFromBuffer (buf, 2 * size);

	for (i = 0; i < amount / 2 ; i ++)  {
	   switch (Mode) {
	      default:
	         case IandQ:
	            V [i] = DSPCOMPLEX (buf [2 * i] / Normalizer,
	                                buf [2 * i + 1] / Normalizer);
	            break;

	         case QandI:
	            V [i] = DSPCOMPLEX (buf [2 * i + 1] / Normalizer,
	                                buf [2 * i] / Normalizer);
	            break;

	         case I_Only:
	            V [i] = DSPCOMPLEX (buf [2 * i] / Normalizer, 0);
	            break;

	         case Q_Only:
	            V [i] = DSPCOMPLEX (buf [2 * i + 1] / Normalizer, 0);
	            break;
	   }
	}
	return amount / 2;
}

int32_t	sdrt9r::Samples	(void) {
	if (!connected)
	   return 0;
	return _I_Buffer	-> GetRingBufferReadAvailable () / 2;
}

//
//	bitDepth is required in the forthcoming 6.1 release
//	In 6.0 it is not called
//	It is used to set the scale for the spectrum
int16_t	sdrt9r::bitDepth	(void) {
	return 24;
}

//
//	the offset is in Hz
void	sdrt9r::setOffset	(int k) {
	vfoOffset	= k;
}

//
//	These functions are typical for network use
void	sdrt9r::readData	(void) {
	while (theSocket -> hasPendingDatagrams ()) {
	   QByteArray datagram;
	   datagram. resize (theSocket -> pendingDatagramSize ());
	   QHostAddress sender;
	   quint16 senderPort;
	   theSocket -> readDatagram (datagram. data (),
	                              datagram. size (),
	                              &sender,
	                              &senderPort);

	   if (dataAllowed)
	      toBuffer (datagram);
	}
}

void	sdrt9r::toBuffer (QByteArray d) {
int32_t	i;
int32_t j;
int32_t	length	= d. size ();
int32_t	buffer [length / 3 ];

	j = 0;
	for (i = 0; i < length; i += 3) {
	   buffer [j] = ((d [i + 2] & 0xFF) << 24) |
	                ((d [i + 1] & 0xFF) << 16) |
	                ((d [i] & 0xFF) << 8);
	   j++;
	}

	_I_Buffer -> putDataIntoBuffer (buffer, length / 3);
	if (_I_Buffer -> GetRingBufferReadAvailable () > theRate / 10)
	   samplesAvailable (theRate / 10);
}

void sdrt9r::sendControlDatagram (void) {
uint8_t freq [4];
QByteArray datagram;
int16_t	i;
int pntr = -1;

	freq [0] = vfoFrequency & 0xFF;  //lsb first
	freq [1] = (vfoFrequency >> 8) & 0xFF;
	freq [2] = (vfoFrequency >> 16) & 0xFF;
	freq [3] = (vfoFrequency >> 24) & 0xFF;

	datagram. resize (16);
	for (i = 0; i < 16 ; i++)
	   datagram[i] = 0;

//	rx
	datagram [++pntr] = freq [0];
	datagram [++pntr] = freq [1];
	datagram [++pntr] = freq [2];
	datagram [++pntr] = freq [3];
//	tx
	datagram [++pntr] = freq [0];
	datagram [++pntr] = freq [1];
	datagram [++pntr] = freq [2];
	datagram [++pntr] = freq [3];

//	rxControl
	char rxControl = (char) 0x01; //rx on
	if (theRate == 96000)
	   rxControl = (char) (rxControl | 0x03);
	if (theRate == 192000)
	   rxControl = (char) (rxControl | 0x04);

	datagram [++pntr] = rxControl;
//	no other setting required... keep it 0!

	theSocket -> writeDatagram (datagram. data (), datagram. size (),
	                            QHostAddress ("169.254.190.182"),
	                            20050);
}

void sdrt9r::broadcastDatagram (void) {
QByteArray datagram;
datagram. resize (63);
int16_t	i;

	for (i = 0; i < 63; i++)
	   datagram [i] = 0;
	datagram [0] = 0xC3;
	datagram [1] = 0x3C;
	datagram [2] = 169;     // Desired Address of SDR Hardware "169.254.190.182"
	datagram [3] = 254;
	datagram [4] = 190;
	datagram [5] = 182;

	udpSocketBroadcasrTx -> writeDatagram (datagram. data (),
	                                       datagram. size (),
	                                       QHostAddress::Broadcast,
	                                       20060);

//	qDebug() << "broadcast sent";
}

void sdrt9r::readBroadcastDatagram (void) {
//    qDebug() << "broadcast received";

	while (udpSocketBroadcastRx -> hasPendingDatagrams ()){
	   QByteArray datagram;
	   QHostAddress sender;
	   quint16 senderPort;
	   datagram. resize (udpSocketBroadcastRx -> pendingDatagramSize ());
	   udpSocketBroadcastRx -> readDatagram (datagram. data (),
	                                         datagram. size (),
	                                         &sender, &senderPort);
//        qDebug() << "Sender = "  + sender.toString() + " en port " + QString("%1").arg(senderPort);

//	check if the data is coming from SDR....
	   if (datagram[0] == (char)0xC3 &&
	       datagram[1] == (char)0x3C) {
	      sdrFound = true;
	   } //break;

//	   if (sdrFound){
//	      qDebug() << "yes sdr found";
//	      break;
//	   }
//	   else
//	      qDebug () << "no sdr not found";
	}
}

//
QT_BEGIN_NAMESPACE
Q_EXPORT_PLUGIN2 (device_sdrt9r, sdrt9r)
QT_END_NAMESPACE

