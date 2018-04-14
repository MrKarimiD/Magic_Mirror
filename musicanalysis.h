#ifndef MUSICANALYSIS_H
#define MUSICANALYSIS_H

#include <QObject>
#include <essentia/algorithmfactory.h>
#include <essentia/essentiamath.h> // for the isSilent function
#include <essentia/pool.h>
#include <QDebug>

using namespace std;
using namespace essentia;
using namespace standard;

class MusicAnalysis : public QObject
{
    Q_OBJECT
public:
    explicit MusicAnalysis(QObject *parent = nullptr);
    vector<Real> beatDetection(QString input, QString output);
    void pitchDetection(QString input, QString output);

signals:

public slots:
};

#endif // MUSICANALYSIS_H
