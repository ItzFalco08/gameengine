#include "json/json.hpp"
#include "string"
#include "glm/glm.hpp"

struct SerializerJSON {
    nlohmann::json& objJson;

    SerializerJSON(nlohmann::json& json): objJson(json) {};

    void Write(const std::string&& key, int value) {
        objJson[key] = value;
    } 
    
    void Write(const std::string&& key, glm::vec3 value) {
        objJson[key] = {value.x, value.y, value.z};
    }
    
    void Write(const std::string&& key, const char* value) {
        objJson[key] = value;
    } 
    
    void Write(const std::string&& key, std::string value) {
        objJson[key] = value;
    } 
    
    void Write(const std::string&& key, float value) {
        objJson[key] = value;
    } 
    
    void Write(const std::string&& key, unsigned int value) {
        objJson[key] = value;
    }
};