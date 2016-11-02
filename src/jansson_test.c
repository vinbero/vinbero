#include <err.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <jansson.h>

int main()
{
    json_error_t error;
    json_t* root = json_load_file("config.json", 0, &error);
    if(root == NULL)
        errx(EXIT_FAILURE, "%s: %u: %s", error.source, error.line, error.text);

    if(json_is_array(root))
        printf("yes it is array\n");
    if(json_array_size(root) < 1)
        errx(EXIT_FAILURE, "minimum size of array is 1");
    json_t* core_args = json_array_get(root, 0);
    json_t* worker_count = json_object_get(core_args, "worker-count");
    printf("%d\n", json_integer_value(worker_count));

    for(size_t i = 1; i != json_array_size(root); ++i)
    {
        json_t* module_args = json_array_get(root, i);
    }


    /*
    json_t* root;
    json_error_t error;
    root = json_loads(text, 0, &error);
    if(!root)
        errx(EXIT_FAILURE, "%s: %u: %s", __FILE__, error.line, error.text);

    if(!json_is_array(root))
    {
        json_decref(root);
        errx(EXIT_FAILURE, "%s: %u: %s", __FILE__, __LINE__, "not an array");
    }

    json_t* data;
    data = json_array_get(root, 0);
    data = json_object_get(data, "key1");
    const char* string;
    string = json_string_value(data);
    printf("%s\n", string);
    json_decref(root);
    */

    return 0;
}

