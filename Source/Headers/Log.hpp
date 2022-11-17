//
// Created by krad on 2022/11/11.
//

#ifndef SNOWCLASSROOM_LOG_HPP
#define SNOWCLASSROOM_LOG_HPP

#include <iostream>
#include <QString>

namespace DEBUG_LOG{
    class Log{
    public:
        enum SIGN_TYPE{
            COMMA = ',',
            DOT = '.',
            SPACE = ' ',
            ENTER = '\n',
            LINE = '-'
        };
        static SIGN_TYPE SPLIT_SIGN;
        static SIGN_TYPE END_SIGN;

        static std::string qstr2stdstr(QString const &s){
            return std::string (s.toLocal8Bit());
        }

        template<typename T>
        void print(T para){
//            std::cout << para << END_SIGN;
            std::cout << para << "\n";

        }

        template<typename T,typename... REST>
        void print(T para1,REST... rest){
//            std::cout << para1 << SPLIT_SIGN;
            std::cout << para1 << ", ";
            print(rest...);
        }

        template<typename T,typename... REST>
        Log(T para1,REST... rest){
            print(para1,rest...);
        }
    };
//    Log::SIGN_TYPE Log::SPLIT_SIGN = Log::SIGN_TYPE::COMMA;
//    Log::SIGN_TYPE Log::END_SIGN = Log::SIGN_TYPE::ENTER;
}




#endif //SNOWCLASSROOM_LOG_HPP
