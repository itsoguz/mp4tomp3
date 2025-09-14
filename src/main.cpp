#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QProcess>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QStandardPaths>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("MP4 to MP3 Converter");
    window.setStyleSheet("background-color: pink;");

    QVBoxLayout *layout = new QVBoxLayout(&window);

    QLabel *titleLabel = new QLabel("MP4 to MP3 Converter", &window);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: blue; font-size: 32px; font-weight: bold; font-family: 'Arial';");
    layout->addWidget(titleLabel);

    layout->addSpacing(30);

    QLabel *instructionLabel = new QLabel("Please Select the Video to Convert", &window);
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setStyleSheet("color: red; font-size: 20px;");
    layout->addWidget(instructionLabel);

    layout->addSpacing(20);

    QPushButton *selectButton = new QPushButton("Select", &window);
    layout->addWidget(selectButton, 0, Qt::AlignCenter);
    selectButton->setFixedSize(100, 40);
    selectButton->setStyleSheet("background-color: gray; color: white;");

    QLabel *fileNameLabel = new QLabel(&window);
    fileNameLabel->setAlignment(Qt::AlignCenter);
    fileNameLabel->setStyleSheet("color: red; font-size: 18px;");
    fileNameLabel->hide();
    layout->addWidget(fileNameLabel);

    QPushButton *convertButton = new QPushButton("Convert", &window);
    convertButton->setFixedSize(100, 40);
    convertButton->setStyleSheet("background-color: gray; color: white;");
    convertButton->hide();
    layout->addWidget(convertButton, 0, Qt::AlignCenter);

    QString selectedFileName;

    // Check if ffmpeg is available
    QString ffmpegPath = QStandardPaths::findExecutable("ffmpeg");
    if (ffmpegPath.isEmpty()) {
        QMessageBox::critical(&window, "Error",
                              "ffmpeg not found! Please install ffmpeg:\n"
                              "sudo dnf install ffmpeg");
        return 1;
    }

    QObject::connect(selectButton, &QPushButton::clicked, [&](){
        // Use home directory as default location on Linux
        QString defaultPath = QDir::homePath();
        selectedFileName = QFileDialog::getOpenFileName(&window,
                                                        "Select MP4 file",
                                                        defaultPath,
                                                        "MP4 Files (*.mp4);;All Files (*)");

        if (!selectedFileName.isEmpty()) {
            // Display just the filename, not the full path for cleaner UI
            QFileInfo fileInfo(selectedFileName);
            fileNameLabel->setText(fileInfo.fileName());
            fileNameLabel->show();
            convertButton->show();
        }
    });

    QObject::connect(convertButton, &QPushButton::clicked, [&](){
        if (!selectedFileName.isEmpty()) {
            fileNameLabel->setText("Converting...");

            // Suggest output filename based on input
            QFileInfo inputInfo(selectedFileName);
            QString suggestedName = inputInfo.path() + "/" +
                                    inputInfo.completeBaseName() + ".mp3";

            QString outputFileName = QFileDialog::getSaveFileName(&window,
                                                                  "Save MP3 file",
                                                                  suggestedName,
                                                                  "MP3 Files (*.mp3);;All Files (*)");

            if (!outputFileName.isEmpty()) {
                QProcess process;
                QStringList args;

                // Simplified ffmpeg arguments that should work universally
                args << "-i" << selectedFileName
                     << "-vn"  // Disable video
                     << "-acodec" << "libmp3lame"  // Use MP3 codec explicitly
                     << "-ab" << "192k"  // Audio bitrate
                     << "-y"  // Overwrite output file if it exists
                     << outputFileName;

                // Debug output
                std::cout << "Running command: " << ffmpegPath.toStdString() << std::endl;
                for (const QString &arg : args) {
                    std::cout << "  " << arg.toStdString() << std::endl;
                }

                process.start(ffmpegPath, args);

                // Better error handling
                if (!process.waitForStarted(5000)) {
                    QMessageBox::critical(&window, "Error",
                                          "Failed to start ffmpeg conversion!\nPath: " + ffmpegPath);
                    fileNameLabel->setText("Conversion failed!");
                    return;
                }

                // Wait for the process to finish (with 5 minute timeout for large files)
                if (!process.waitForFinished(300000)) {
                    process.terminate();
                    QMessageBox::critical(&window, "Error",
                                          "Conversion timed out or failed!");
                    fileNameLabel->setText("Conversion failed!");
                    return;
                }

                // Read both stdout and stderr
                QString output = process.readAllStandardOutput();
                QString error = process.readAllStandardError();

                // Check exit status
                if (process.exitStatus() == QProcess::NormalExit &&
                    process.exitCode() == 0) {
                    fileNameLabel->setText("Converted successfully!");
                    QMessageBox::information(&window, "Success",
                                             "File converted successfully!\n" + outputFileName);
                } else {
                    // Show detailed error information
                    QString errorMsg = QString("Conversion failed!\n\n"
                                               "Exit code: %1\n\n"
                                               "Error output:\n%2\n\n"
                                               "Standard output:\n%3")
                                           .arg(process.exitCode())
                                           .arg(error.left(500))  // Limit error message length
                                           .arg(output.left(500));

                    QMessageBox::critical(&window, "Error", errorMsg);
                    fileNameLabel->setText("Conversion failed!");

                    // Also print to console for debugging
                    std::cerr << "FFmpeg error output:" << std::endl;
                    std::cerr << error.toStdString() << std::endl;
                }
            }
        }
    });

    window.setLayout(layout);
    window.resize(400, 300);  // Set a default window size
    window.show();

    return app.exec();
}
