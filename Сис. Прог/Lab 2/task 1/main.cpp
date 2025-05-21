#include <iostream>
#include "logger.h"
int main() {
    Logger::Builder builder;
    builder.addFileHandler("app.log")
            .addStreamHandler(std::cout)
            .setLevel(Level::WARNING);

    Logger logger = builder.build();

    logger.debug("Это сообщение отладки не будет записано");
    logger.warning("Предупреждение: что-то пошло не так");
    logger.error("Ошибка: критическая проблема");
    logger.critical("Критическая ошибка: необходимо вмешательство");

    logger.close();

    return 0;
}
