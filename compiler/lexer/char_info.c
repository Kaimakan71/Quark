/*
 * Character info table.
 * Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
 * Provided under the BSD 3-Clause license.
 */
#include <lexer/char_info.h>

uint16_t char_info[256] = {
        /*
                NUL SOH STX ETX
                EOT ENQ ACK BEL
        */
        0           , 0           , 0           , 0           ,
        0           , 0           , 0           , 0           ,
        /*
                BS  TAB LF  VT
                FF  CR  SO  SI
        */
        0           , CHAR_HORZ_WS, CHAR_VERT_WS, CHAR_VERT_WS,
        CHAR_VERT_WS, CHAR_HORZ_WS, 0           , 0           ,
        /*
                DLE DC1 DC2 DC3
                DC4 NAK SYN ETB
        */
        0           , 0           , 0           , 0           ,
        0           , 0           , 0           , 0           ,
        /*
                CAN EM  SUB ESC
                FS  GS  RS  US
        */
        0           , 0           , 0           , 0           ,
        0           , 0           , 0           , 0           ,
        /*
                 !"#
                $%&'
        */
        CHAR_HORZ_WS, CHAR_OPER   , 0           , 0           ,
        0           , CHAR_OPER   , CHAR_OPER   , 0           ,
        /*
                ()*+
                ,-./
        */
        CHAR_LPAREN , CHAR_RPAREN , CHAR_OPER   , CHAR_OPER   ,
        CHAR_COMMA  , CHAR_OPER   , CHAR_DOT    , CHAR_OPER   ,
        /*
                0123
                4567
        */
        CHAR_DIGIT  , CHAR_DIGIT  , CHAR_DIGIT  , CHAR_DIGIT  ,
        CHAR_DIGIT  , CHAR_DIGIT  , CHAR_DIGIT  , CHAR_DIGIT  ,
        /*
                89:;
                <=>?
        */
        CHAR_DIGIT  , CHAR_DIGIT  , CHAR_COLON  , CHAR_SEMI   ,
        CHAR_OPER   , CHAR_OPER   , CHAR_OPER   , 0           ,
        /*
                @ABC
                DEFG
        */
        0           , CHAR_XUPPER , CHAR_XUPPER , CHAR_XUPPER ,
        CHAR_XUPPER , CHAR_XUPPER , CHAR_XUPPER , CHAR_UPPER  ,
        /*
                HIJK
                LMNO
        */
        CHAR_UPPER  , CHAR_UPPER  , CHAR_UPPER  , CHAR_UPPER  ,
        CHAR_UPPER  , CHAR_UPPER  , CHAR_UPPER  , CHAR_UPPER  ,
        /*
                PQRS
                TUVW
        */
        CHAR_UPPER  , CHAR_UPPER  , CHAR_UPPER  , CHAR_UPPER  ,
        CHAR_UPPER  , CHAR_UPPER  , CHAR_UPPER  , CHAR_UPPER  ,
        /*
                XYZ[
                \]^_
        */
        CHAR_UPPER  , CHAR_UPPER  , CHAR_UPPER  , CHAR_LSQUARE,
        0           , CHAR_RSQUARE, CHAR_OPER   , 0           ,
        /*
                `abc
                defg
        */
        0           , CHAR_XLOWER , CHAR_XLOWER , CHAR_XLOWER ,
        CHAR_XLOWER , CHAR_XLOWER , CHAR_XLOWER , CHAR_LOWER  ,
        /*
                hijk
                lmno
        */
        CHAR_LOWER  , CHAR_LOWER  , CHAR_LOWER  , CHAR_LOWER  ,
        CHAR_LOWER  , CHAR_LOWER  , CHAR_LOWER  , CHAR_LOWER  ,
        /*
                pqrs
                tuvw
        */
        CHAR_LOWER  , CHAR_LOWER  , CHAR_LOWER  , CHAR_LOWER  ,
        CHAR_LOWER  , CHAR_LOWER  , CHAR_LOWER  , CHAR_LOWER  ,
        /*
                xyz{
                |}~
        */
        CHAR_LOWER  , CHAR_LOWER  , CHAR_LOWER  , CHAR_LCURLY ,
        CHAR_OPER   , CHAR_RCURLY , CHAR_OPER   , 0           ,
};
