#include "includes.h"    
void mqtt_cjson_parse(const char *json_string,char *s)
{
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error before: %s\n", error_ptr);
        }
        return ;
    }
	//在 JSON 对象 (object) 中，根据键名 (string) 精确匹配（区分大小写）对应的字段
    cJSON *results = cJSON_GetObjectItemCaseSensitive(root, "nowtime");
            if (cJSON_IsString(results) && (results->valuestring != NULL)) 
							{
								strcpy(s,results->valuestring);
							}
	 cJSON_Delete(root);

}
void mqtt_cjson_parse1(const char *json_string,char *s)
{
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error before: %s\n", error_ptr);
        }
        return ;
    }
	//在 JSON 对象 (object) 中，根据键名 (string) 精确匹配（区分大小写）对应的字段
    cJSON *results = cJSON_GetObjectItemCaseSensitive(root, "results");
    if (cJSON_IsArray(results)) { 
        cJSON *first_result = cJSON_GetArrayItem(results, 0);
        if (first_result != NULL) {
            cJSON *now = cJSON_GetObjectItemCaseSensitive(first_result, "now");
            if (now != NULL) {
                cJSON *text = cJSON_GetObjectItemCaseSensitive(now, "text");
                if (cJSON_IsString(text) && (text->valuestring != NULL)) {
										strcpy(s,text->valuestring);
                }
            }
        }
    }
	 cJSON_Delete(root);
	
	printf("\r\n");

}
