class SensorInterface {
public:
    virtual void init() = 0;
    virtual bool test() = 0;
    virtual bool readValues() = 0;
};
