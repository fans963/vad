#include "SpeakerDialog.h"
#include "../core/AudioDecoder.h"
#include "../dsp/DspUtils.h"
#include "../dsp/FftProcessor.h"

#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSlider>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <algorithm>
#include <cmath>
#include <limits>

namespace {
constexpr int kFrameSize = 256, dimension = 13;
double distance(const QVector<double>& a, const QVector<double>& b) {
    double sum = 0;
    for (int i = 0; i < a.size(); ++i) { const double d = a[i]-b[i]; sum += d*d; }
    return sum;
}
}

SpeakerDialog::SpeakerDialog(QWidget* parent) : QDialog(parent) {
    setupUi(); loadModels(); refresh();
}

void SpeakerDialog::setupUi() {
    setWindowTitle(QStringLiteral("说话人识别与确认（GMM）")); resize(760, 480);
    auto* root = new QHBoxLayout(this);
    auto* library = new QGroupBox(QStringLiteral("说话人信息"));
    auto* left = new QVBoxLayout(library); m_speakers = new QListWidget; left->addWidget(m_speakers, 1);
    auto* nameRow = new QHBoxLayout; m_name = new QLineEdit; m_name->setPlaceholderText(QStringLiteral("姓名"));
    auto* remove = new QPushButton(QStringLiteral("删除")); nameRow->addWidget(m_name, 1); nameRow->addWidget(remove);
    left->addLayout(nameRow); root->addWidget(library, 1);

    auto* right = new QVBoxLayout; auto* options = new QGroupBox(QStringLiteral("声音样本与建模参数"));
    auto* form = new QFormLayout(options);
    auto fileRow = [](QLineEdit*& edit, QPushButton*& button) {
        auto* row = new QHBoxLayout; edit = new QLineEdit; edit->setReadOnly(true);
        button = new QPushButton(QStringLiteral("选择...")); row->addWidget(edit, 1); row->addWidget(button); return row;
    };
    QPushButton *chooseTrain, *chooseTest;
    form->addRow(QStringLiteral("训练音频："), fileRow(m_trainFiles, chooseTrain));
    form->addRow(QStringLiteral("测试音频："), fileRow(m_testFile, chooseTest));
    m_order = new QComboBox; for (int n : {2,4,8,16,32,64}) m_order->addItem(QString::number(n), n);
    m_order->setCurrentText(QStringLiteral("16")); form->addRow(QStringLiteral("GMM 阶数："), m_order);
    auto* thresholdRow = new QHBoxLayout; m_threshold = new QSlider(Qt::Horizontal);
    m_threshold->setRange(-2000, 0); m_threshold->setValue(-500); m_thresholdText = new QLabel(QStringLiteral("-50.0"));
    thresholdRow->addWidget(m_threshold, 1); thresholdRow->addWidget(m_thresholdText);
    form->addRow(QStringLiteral("确认门限："), thresholdRow); right->addWidget(options);
    auto* buttons = new QHBoxLayout; auto* train = new QPushButton(QStringLiteral("训练模型"));
    auto* identifyButton = new QPushButton(QStringLiteral("说话人辨认")); auto* verifyButton = new QPushButton(QStringLiteral("说话人确认"));
    buttons->addWidget(train); buttons->addWidget(identifyButton); buttons->addWidget(verifyButton); right->addLayout(buttons);
    m_result = new QLabel(QStringLiteral("请选择样本。")); m_result->setWordWrap(true);
    m_result->setFrameShape(QFrame::StyledPanel); m_result->setAlignment(Qt::AlignTop); right->addWidget(m_result, 1);
    root->addLayout(right, 3);
    connect(chooseTrain,&QPushButton::clicked,this,&SpeakerDialog::chooseTrainingFiles);
    connect(chooseTest,&QPushButton::clicked,this,&SpeakerDialog::chooseTestFile);
    connect(train,&QPushButton::clicked,this,&SpeakerDialog::trainModel);
    connect(identifyButton,&QPushButton::clicked,this,&SpeakerDialog::identify);
    connect(verifyButton,&QPushButton::clicked,this,&SpeakerDialog::verify);
    connect(remove,&QPushButton::clicked,this,&SpeakerDialog::removeSpeaker);
    connect(m_speakers,&QListWidget::currentTextChanged,m_name,&QLineEdit::setText);
    connect(m_threshold,&QSlider::valueChanged,this,[this](int n){m_thresholdText->setText(QString::number(n/10.0,'f',1));});
}

void SpeakerDialog::chooseTrainingFiles() {
    m_trainingPaths=QFileDialog::getOpenFileNames(this,QStringLiteral("选择训练语音"),{},QStringLiteral("音频 (*.wav *.flac *.ogg)"));
    m_trainFiles->setText(m_trainingPaths.join(QStringLiteral("; ")));
}
void SpeakerDialog::chooseTestFile() {
    m_testPath=QFileDialog::getOpenFileName(this,QStringLiteral("选择测试语音"),{},QStringLiteral("音频 (*.wav *.flac *.ogg)"));
    m_testFile->setText(m_testPath);
}

QVector<QVector<double>> SpeakerDialog::features(const QStringList& files) const {
    QVector<QVector<double>> out; AudioDecoder decoder;
    for (const auto& path:files) { const auto audio=decoder.decodeFile(path);
        for (const auto& frame:frameSignal(audio.samples,kFrameSize,true,true,0.95f)) {
            FftProcessor fft(kFrameSize); const auto values=fft.computeMFCC(frame,audio.info.sampleRate,24,dimension);
            QVector<double> row; row.reserve(dimension); for(float v:values) row.append(v); out.append(std::move(row));
        }
    } return out;
}

SpeakerDialog::Model SpeakerDialog::train(const QString& name,const QVector<QVector<double>>& data,int order) const {
    Model model; model.name=name; const int k=std::clamp(order,1,int(data.size())); model.components.resize(k);
    for(int c=0;c<k;++c) model.components[c].mean=data[(qint64(c)*data.size())/k];
    QVector<int> assignment(data.size(),-1);
    for(int iteration=0;iteration<30;++iteration) { bool changed=false;
        for(int i=0;i<data.size();++i) { int best=0; double bestValue=distance(data[i],model.components[0].mean);
            for(int c=1;c<k;++c) { double value=distance(data[i],model.components[c].mean); if(value<bestValue){best=c;bestValue=value;} }
            changed|=assignment[i]!=best; assignment[i]=best;
        }
        QVector<int> count(k); QVector<QVector<double>> sums(k,QVector<double>(dimension));
        for(int i=0;i<data.size();++i){++count[assignment[i]];for(int d=0;d<dimension;++d)sums[assignment[i]][d]+=data[i][d];}
        for(int c=0;c<k;++c)if(count[c])for(int d=0;d<dimension;++d)model.components[c].mean[d]=sums[c][d]/count[c];
        if(!changed&&iteration)break;
    }
    QVector<int> count(k); for(auto& c:model.components)c.variance.fill(1e-3,dimension);
    for(int i=0;i<data.size();++i){auto& c=model.components[assignment[i]];++count[assignment[i]];
        for(int d=0;d<dimension;++d){double delta=data[i][d]-c.mean[d];c.variance[d]+=delta*delta;}}
    for(int c=0;c<k;++c){model.components[c].weight=std::max(1,count[c])/double(data.size());
        for(double& v:model.components[c].variance)v=std::max(v/std::max(1,count[c]),1e-4);}
    return model;
}

double SpeakerDialog::score(const Model& model,const QVector<QVector<double>>& data) const {
    if(model.components.isEmpty()||data.isEmpty())return -std::numeric_limits<double>::infinity(); double total=0;
    for(const auto& row:data){QVector<double> scores;double maximum=-std::numeric_limits<double>::infinity();
        for(const auto& c:model.components){double value=std::log(std::max(c.weight,1e-12));
            for(int d=0;d<dimension;++d){double v=c.variance[d],delta=row[d]-c.mean[d];value-=.5*(std::log(2*M_PI*v)+delta*delta/v);}
            scores.append(value);maximum=std::max(maximum,value);}
        double sum=0;for(double value:scores)sum+=std::exp(value-maximum);total+=maximum+std::log(sum);
    } return total/data.size();
}

void SpeakerDialog::trainModel(){QString name=m_name->text().trimmed();if(name.isEmpty()||m_trainingPaths.isEmpty()){
    QMessageBox::warning(this,QStringLiteral("训练"),QStringLiteral("请输入姓名并选择训练音频。"));return;}
    auto data=features(m_trainingPaths);if(data.size()<8){QMessageBox::warning(this,QStringLiteral("训练"),QStringLiteral("有效语音帧过少。"));return;}
    Model value=train(name,data,m_order->currentData().toInt());auto found=std::find_if(m_models.begin(),m_models.end(),[&](const Model& m){return m.name==name;});
    if(found==m_models.end())m_models.append(std::move(value));else *found=std::move(value);saveModels();refresh();
    m_result->setText(QStringLiteral("已为“%1”训练 %2 阶 GMM，共 %3 帧 MFCC。").arg(name).arg(m_order->currentData().toInt()).arg(data.size()));}

void SpeakerDialog::identify(){if(m_testPath.isEmpty()||m_models.isEmpty()){QMessageBox::warning(this,QStringLiteral("辨认"),QStringLiteral("请选择测试音频并训练模型。"));return;}
    auto data=features({m_testPath});QVector<QPair<QString,double>> values;for(const auto& model:m_models)values.append({model.name,score(model,data)});
    std::sort(values.begin(),values.end(),[](const auto&a,const auto&b){return a.second>b.second;});QString text=QStringLiteral("辨认结果：%1\n\n").arg(values.first().first);
    for(const auto& [name,value]:values)text+=QStringLiteral("%1\t%2\n").arg(name).arg(value,0,'f',3);m_result->setText(text);}

void SpeakerDialog::verify(){QString name=m_speakers->currentItem()?m_speakers->currentItem()->text():m_name->text().trimmed();
    auto model=std::find_if(m_models.begin(),m_models.end(),[&](const Model&m){return m.name==name;});if(model==m_models.end()||m_testPath.isEmpty()){
        QMessageBox::warning(this,QStringLiteral("确认"),QStringLiteral("请选择说话人和测试音频。"));return;}
    double value=score(*model,features({m_testPath})),threshold=m_threshold->value()/10.0;
    m_result->setText(QStringLiteral("申请身份：%1\n对数似然：%2\n门限：%3\n\n%4").arg(name).arg(value,0,'f',3).arg(threshold,0,'f',1)
        .arg(value>=threshold?QStringLiteral("被接受"):QStringLiteral("被拒绝")));}

void SpeakerDialog::removeSpeaker(){if(!m_speakers->currentItem())return;QString name=m_speakers->currentItem()->text();
    for (qsizetype i = m_models.size() - 1; i >= 0; --i)
        if (m_models[i].name == name) m_models.removeAt(i);
    saveModels();refresh();}
QString SpeakerDialog::modelPath()const{QString dir=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);QDir().mkpath(dir);return dir+QStringLiteral("/speaker_models.json");}
void SpeakerDialog::refresh(){m_speakers->clear();for(const auto&m:m_models)m_speakers->addItem(m.name);}

bool SpeakerDialog::saveModels()const{QJsonArray models;for(const auto&m:m_models){QJsonArray components;for(const auto&c:m.components){QJsonArray mean,var;
    for(double v:c.mean)mean.append(v);for(double v:c.variance)var.append(v);components.append(QJsonObject{{"weight",c.weight},{"mean",mean},{"variance",var}});}
    models.append(QJsonObject{{"name",m.name},{"components",components}});}QFile file(modelPath());return file.open(QIODevice::WriteOnly|QIODevice::Truncate)&&file.write(QJsonDocument(models).toJson())>=0;}
void SpeakerDialog::loadModels(){QFile file(modelPath());if(!file.open(QIODevice::ReadOnly))return;for(const auto&item:QJsonDocument::fromJson(file.readAll()).array()){
    auto object=item.toObject();Model m;m.name=object["name"].toString();for(const auto&entry:object["components"].toArray()){auto o=entry.toObject();Component c;c.weight=o["weight"].toDouble();
        for(auto v:o["mean"].toArray())c.mean.append(v.toDouble());for(auto v:o["variance"].toArray())c.variance.append(v.toDouble());if(c.mean.size()==dimension&&c.variance.size()==dimension)m.components.append(std::move(c));}
    if(!m.name.isEmpty()&&!m.components.isEmpty())m_models.append(std::move(m));}}
