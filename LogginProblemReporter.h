#pragma once

#include <string>
#include <osmium/area/assembler.hpp>

#define ELPP_NO_DEFAULT_LOG_FILE
#include "easylogging++.h"

using namespace osmium;

class LoggingProblemReporter : public area::ProblemReporter {

    void report_duplicate_node(object_id_type node_id1, object_id_type node_id2, Location) override {
        LOG(WARNING) << "Duplicate nodes (same location): " << node_id1 << " and " << node_id2;
    }

    virtual void report_touching_ring(object_id_type node_id, Location location) {
        LOG(WARNING) << "Rings are touching at node id " << node_id << " with lon/lat " << location.lon() << "/" <<
        location.lat();
    }

    virtual void report_duplicate_segment(const NodeRef &nr1, const NodeRef &nr2) {
        LOG(WARNING) << "Duplicate segments " << nr1.ref() << " and " << nr2.ref();
    }

    virtual void report_ring_not_closed(const NodeRef &nr, const Way *) {
        LOG(WARNING) << "Open ring " << nr.ref();
    }

    virtual void report_role_should_be_outer(object_id_type way_id, Location seg_start, Location seg_end) {
        LOG(WARNING) << "Way with id " << way_id << " has segment with wrong role (should be 'outer'). Start: " <<
        seg_start << ", End: " << seg_end;
    }

    virtual void report_role_should_be_inner(object_id_type way_id, Location seg_start, Location seg_end) {
        LOG(WARNING) << "Way with id " << way_id << " has segment with wrong role (should be 'inner'). Start: " <<
        seg_start << ", End: " << seg_end;
    }
};
