// g2o - General Graph Optimization
// Copyright (C) 2011 R. Kuemmerle, G. Grisetti, W. Burgard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Modified by Michal Nowicki (michal.nowicki@put.poznan.pl) 2015
// Added EdgeSE2PointXYZfixedZDistance (Distance measurement)

#include "edge_se2_pointXYZ_fixedZ_distance.h"

#ifdef G2O_HAVE_OPENGL
#include "g2o/stuff/opengl_wrapper.h"
#include "g2o/stuff/opengl_primitives.h"
#endif

namespace g2o {

EdgeSE2PointXYZfixedZDistance::EdgeSE2PointXYZfixedZDistance() {
	frequency = 2412;
}

EdgeSE2PointXYZfixedZDistance::EdgeSE2PointXYZfixedZDistance(
		double _frequency) {
	frequency = _frequency;
}

void EdgeSE2PointXYZfixedZDistance::initialEstimate(
		const OptimizableGraph::VertexSet& from,
		OptimizableGraph::Vertex* /*to*/) {
	assert(
			from.size() == 1 && from.count(_vertices[0]) == 1
					&& "Can not initialize VertexSE2 position by VertexPointXY");

	if (from.count(_vertices[0]) != 1)
		return;
	double r = 2.;
	const VertexSE2* v1 = static_cast<const VertexSE2*>(_vertices[0]);
	VertexPointXYZ* l2 = static_cast<VertexPointXYZ*>(_vertices[1]);
	SE2 t = v1->estimate();
	Vector3d vr;
	vr[0] = t.translation().x();
	vr[1] = t.translation().y();
	vr[2] = 10.0;
	l2->setEstimate(vr);
}

bool EdgeSE2PointXYZfixedZDistance::read(std::istream& is) {
	is >> _measurement >> information()(0, 0);
	return true;
}

bool EdgeSE2PointXYZfixedZDistance::write(std::ostream& os) const {
	os << measurement() << " " << information()(0, 0);
	return os.good();
}

EdgeSE2PointXYZfixedZDistanceWriteGnuplotAction::EdgeSE2PointXYZfixedZDistanceWriteGnuplotAction() :
		WriteGnuplotAction(typeid(EdgeSE2PointXYZfixedZDistance).name()) {
}

HyperGraphElementAction* EdgeSE2PointXYZfixedZDistanceWriteGnuplotAction::operator()(
		HyperGraph::HyperGraphElement* element,
		HyperGraphElementAction::Parameters* params_) {
	if (typeid(*element).name() != _typeName)
		return 0;
	WriteGnuplotAction::Parameters* params =
			static_cast<WriteGnuplotAction::Parameters*>(params_);
	if (!params->os) {
		std::cerr << __PRETTY_FUNCTION__ << ": warning, on valid os specified"
				<< std::endl;
		return 0;
	}

	EdgeSE2PointXYZfixedZDistance* e =
			static_cast<EdgeSE2PointXYZfixedZDistance*>(element);
	VertexSE2* fromEdge = static_cast<VertexSE2*>(e->vertex(0));
	VertexPointXYZ* toEdge = static_cast<VertexPointXYZ*>(e->vertex(1));
	*(params->os) << fromEdge->estimate().translation().x() << " "
			<< fromEdge->estimate().translation().y() << " "
			<< fromEdge->estimate().rotation().angle() << std::endl;
	*(params->os) << toEdge->estimate().x() << " " << toEdge->estimate().y()
			<< std::endl;
	*(params->os) << std::endl;
	return this;
}

#ifdef G2O_HAVE_OPENGL
EdgeSE2PointXYDistanceDrawAction::EdgeSE2PointXYZfixedZDistanceDrawAction(): DrawAction(typeid(EdgeSE2PointXYZfixedZDistance).name()) {}

HyperGraphElementAction* EdgeSE2PointXYZfixedZDistanceDrawAction::operator()(HyperGraph::HyperGraphElement* element, HyperGraphElementAction::Parameters* params_) {
	if (typeid(*element).name()!=_typeName)
	return 0;

	refreshPropertyPtrs(params_);
	if (! _previousParams)
	return this;

	if (_show && !_show->value())
	return this;

	EdgeSE2PointXYDistance* e = static_cast<EdgeSE2PointXYDistance*>(element);
	VertexSE2* from = static_cast<VertexSE2*>(e->vertex(0));
	VertexPointXY* to = static_cast<VertexPointXY*>(e->vertex(1));
	if (! from)
	return this;
	double guessRange=5;
	double theta = e->measurement();
	Vector2D p(cos(theta)*guessRange, sin(theta)*guessRange);
	glPushAttrib(GL_ENABLE_BIT|GL_LIGHTING|GL_COLOR);
	glDisable(GL_LIGHTING);
	if (!to) {
		p=from->estimate()*p;
		glColor3f(LANDMARK_EDGE_GHOST_COLOR);
		glPushAttrib(GL_POINT_SIZE);
		glPointSize(3);
		glBegin(GL_POINTS);
		glVertex3f((float)p.x(),(float)p.y(),0.f);
		glEnd();
		glPopAttrib();
	} else {
		p=to->estimate();
		glColor3f(LANDMARK_EDGE_COLOR);
	}
	glBegin(GL_LINES);
	glVertex3f((float)from->estimate().translation().x(),(float)from->estimate().translation().y(),0.f);
	glVertex3f((float)p.x(),(float)p.y(),0.f);
	glEnd();
	glPopAttrib();
	return this;
}
#endif

}
 // end namespace
