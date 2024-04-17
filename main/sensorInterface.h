class SensorInterface {
public:
    virtual void init() = 0;
    virtual bool test() = 0;
    virtual bool readValues(StaticJsonDocument<JSON_OBJECT_SIZE(MAX_VALUES_READ)>& values) = 0;
};
