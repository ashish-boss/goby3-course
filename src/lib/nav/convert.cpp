#include "convert.h"

std::string goby3_course::node_report_from_nav(
    const goby::middleware::frontseat::protobuf::NodeStatus& node_status)
{
    NodeRecord node_report(
        node_status.name(),
        goby::middleware::frontseat::protobuf::VehicleType_Name(node_status.type()));
    node_report.setX(node_status.local_fix().x());
    node_report.setY(node_status.local_fix().y());
    node_report.setLat(node_status.global_fix().lat());
    node_report.setLon(node_status.global_fix().lon());
    node_report.setSpeed(node_status.speed().over_ground());
    node_report.setHeading(node_status.pose().heading());
    node_report.setDepth(node_status.global_fix().depth());
    node_report.setTimeStamp(node_status.time());
    
    return node_report.getSpec();
}
