/**
   \file
   \author Shin'ichiro Nakaoka
*/

#include "BasicSensorSimulationHelper.h"
#include "Link.h"
#include <Eigen/StdVector>

using namespace std;
using namespace boost;
using namespace cnoid;

namespace cnoid {

class BasicSensorSimulationHelperImpl
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    BasicSensorSimulationHelper* self;
    BodyPtr body;

    // preview control gain matrices for acceleration sensors
    Matrix2 A;
    Vector2 B;

    // gravity acceleration
    Vector3 g;

    struct KFState
    {
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        Vector2 x[3];
    };
    typedef vector<KFState, Eigen::aligned_allocator<KFState> > KFStateArray;

    KFStateArray kfStates;

    BasicSensorSimulationHelperImpl(BasicSensorSimulationHelper* self);
    void initialize(BodyPtr body, double timeStep, const Vector3& gravityAcceleration);
};
}

namespace {
typedef BasicSensorSimulationHelperImpl Impl;
}
    

BasicSensorSimulationHelper::BasicSensorSimulationHelper()
{
    isActive_ = false;
    impl = new BasicSensorSimulationHelperImpl(this);
}


BasicSensorSimulationHelperImpl::BasicSensorSimulationHelperImpl(BasicSensorSimulationHelper* self)
    : self(self)
{

}


BasicSensorSimulationHelper::~BasicSensorSimulationHelper()
{
    delete impl;
}


void BasicSensorSimulationHelper::initialize
(BodyPtr body, double timeStep, const Vector3& gravityAcceleration)
{
    const DeviceList<>& devices = body->devices();
    DeviceList<Sensor> sensors;
    sensors.reserve(devices.size());
    sensors = devices;

    if(sensors.empty()){
        isActive_ = false;

    } else {
        forceSensors_ = sensors;
        gyroSensors_ = sensors;
        accelSensors_ = sensors;
        
        impl->initialize(body, timeStep, gravityAcceleration);

        isActive_ = true;
    }
}


void Impl::initialize(BodyPtr body, double timeStep, const Vector3& gravityAcceleration)
{
    this->body = body;
    g = gravityAcceleration;

    const DeviceList<AccelSensor>& accelSensors = self->accelSensors_;
    if(accelSensors.empty()){
        return;
    }

    // Kalman filter design
    static const double n_input = 100.0;  // [N]
    static const double n_output = 0.001; // [m/s]

    // Analytical solution of Kalman filter (continuous domain)
    // s.kajita  2003 Jan.22

    Matrix2 Ac;
    Ac <<
        -sqrt(2.0 * n_input / n_output), 1.0,
        -n_input / n_output            , 0.0;

    Vector2 Bc(sqrt(2.0 * n_input / n_output), n_input / n_output);

    A.setIdentity();
    Matrix2 An = Matrix2::Identity();
    Matrix2 An2;
    B = timeStep * Bc;
    Vector2 Bn = B;
    Vector2 Bn2;

    double factorial[14];
    double r = 1.0;
    factorial[1] = r;
    for(int i=2; i <= 13; ++i){
        r += 1.0;
        factorial[i] = factorial[i-1] * r;
    }

    for(int i=1; i <= 12; i++){
        An2.noalias() = Ac * An;
        An = timeStep * An2;
        A += (1.0 / factorial[i]) * An;
                
        Bn2.noalias() = Ac * Bn;
        Bn = timeStep * Bn2;
        B += (1.0 / factorial[i+1]) * Bn;
    }

    // initialize kalman filtering
    kfStates.resize(accelSensors.size());
    for(int i=0; i < accelSensors.size(); ++i){
        const AccelSensor* sensor = accelSensors.get(i);
        const Link* link = sensor->link();
        const Vector3 o_Vgsens = link->R() * (link->R().transpose() * link->w()).cross(sensor->localTranslation()) + link->v();
        KFState& s = kfStates[i];
        for(int i=0; i < 3; ++i){
            s.x[i](0) = o_Vgsens(i);
            s.x[i](1) = 0.0;
        }
    }
}


void BasicSensorSimulationHelper::updateGyroAndAccelSensors()
{
    for(size_t i=0; i < gyroSensors_.size(); ++i){
        RateGyroSensor* gyro = gyroSensors_.get(i);
        const Link* link = gyro->link();
        gyro->w() = gyro->R_local().transpose() * link->R().transpose() * link->w();
        gyro->notifyStateChange();
    }

    if(!accelSensors_.empty()){

        const Matrix2& A = impl->A;
        const Vector2& B = impl->B;
        const Vector3& g = impl->g;

        for(size_t i=0; i < accelSensors_.size(); ++i){

            AccelSensor* sensor = accelSensors_.get(i);
            const Link* link = sensor->link();
            
            // kalman filtering
            Impl::KFState& s = impl->kfStates[i];
            const Vector3 o_Vgsens = link->R() * (link->R().transpose() * link->w()).cross(sensor->p_local()) + link->v();
            for(int i=0; i < 3; ++i){
                s.x[i] = A * s.x[i] + o_Vgsens(i) * B;
            }
            
            Vector3 o_Agsens(s.x[0](1), s.x[1](1), s.x[2](1));
            o_Agsens -= g;
            
            sensor->dv() = link->R().transpose() * o_Agsens;
            sensor->notifyStateChange();
        }
    }
}
