#include <stdbool.h>

enum Parsed {
    GOOD  = 0,
    ERROR = 1,
    NEXT  = 2,
};

enum Block_Type {
    IN_PORT  = 0,
    SUM      = 1,
    GAIN     = 2,
    DELAY    = 3,
    OUT_PORT = 4,
    COUNT    = 5,
};

struct Block {
    
};

struct Parser {
    std::vector<Block> blocks;
    bool parsed_attributes;
    // str tag_name;
    // str attr_name;
    // str attr_value;
};

#define report_error(fmt, ...) \
    printf("ERROR: " fmt "\n", ...)

Parsed parse_block(Parser *parser) {
    if (parser->parsed_attributes) {
        report_error("block has no attributes, but it should have BlockType, Name and SID");
        return ERROR;
    }

    Block_Type block_type = COUNT;
    str        block_name = {0};
    u16        block_id   = -1;
    while (1) {
        str name = {0};
        str value = {0};
        Parsed result = tag_attribute(parser, &name, &value);
        if (result == ERROR) return ERROR;
        if (result == NEXT) break;

        if (name == "BlockType") {
            block_type = str_to_block_type(value);
        } else if (name == "Name") {
            block_name = value;
        } else if (name == "SID") {
            if (str_to_int(*value, &block_id)) return ERROR;
        } else {
            
        }
    }


    if (type == COUNT) return ERROR;

}

Parsed parse(Parser *parser) {
    if (xml_header(parser)) return ERRPR;

    str name = {0};
    if (start_tag(parser, &name)) return ERROR;

    while (end_tag(parser, &name) == ERROR) {
        str name = {0};
        if (start_tag(parser, &name)) return ERROR;

        if (name == "Block") {
            if (parse_block(parser)) return ERROR;
        } else if (name == "Line") {
            if (parse_line(parser);
        } else {
            report_error("tag %s is not expected, only Block and Line are", name);
            return ERROR;
        }

        if (end_tag(parser, &name)) return ERROR;
        // ^ if (parser->tag_name != tag_name) {
        //     printf("ERROR" End tag name does not match start tag name\n");
        //     return ERROR;
        // }
    }

    return GOOD;
}
