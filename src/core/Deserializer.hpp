#include "json/json.hpp"
#include "string"
#include "glm/glm.hpp"

struct DeserializerJSON {
    nlohmann::json& jsonObj;

    DeserializerJSON(nlohmann::json& json) : jsonObj(json) {};
    
    int ReadInt(std::string key) { return jsonObj.at(key).get<int>(); }
    float ReadFloat(std::string key) { return jsonObj.at(key).get<int>(); }
    bool ReadBool(std::string key) { return jsonObj.at(key).get<bool>(); }
    std::string ReadString(std::string key) { return jsonObj.at(key).get<std::string>(); }
    glm::vec3 ReadVec3(std::string key) {
        auto vec = jsonObj.at(key);
        return {vec[0], vec[1], vec[2]};
    }
};