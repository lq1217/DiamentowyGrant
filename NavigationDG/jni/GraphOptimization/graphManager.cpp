#include "graphManager.h"

GraphManager::GraphManager() {
	// Creating needed mutexes
	if (pthread_mutex_init(&graphMtx, NULL))
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Mutex init failed!");

	if (pthread_mutex_init(&verticesMtx, NULL))
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Mutex init failed!");

	if (pthread_mutex_init(&bufferMtx, NULL))
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Mutex init failed!");

	if (pthread_mutex_init(&addMtx, NULL))
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Mutex init failed!");

	// Initial assumption is that we start at (0,0,0);
	prevUserPositionTheta = prevUserPositionX = prevUserPositionY = 0.0;

	// Creating PCG solver
	BlockSolverX::LinearSolverType* linearSolver = new LinearSolverPCG<
			BlockSolverX::PoseMatrixType>();
	BlockSolverX* blockSolver = new BlockSolverX(linearSolver);

	// Creating Levenber algorithm
	OptimizationAlgorithmLevenberg* optimizationAlgorithm =
			new OptimizationAlgorithmLevenberg(blockSolver);

	// Setting Levenberg as optimizaiton algorithm
	optimizer.setAlgorithm(optimizationAlgorithm);

	// Adding initial node (0,0,0)
	stringstream tmp;
	tmp << "0 0.0 0.0 0.0\n";
	addVertex(tmp, 0);
}


int GraphManager::optimize(int iterationCount) {
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "GraphManager::optimize");

	// Locking the graph - we are the only one modifying
	pthread_mutex_lock(&graphMtx);

	// No edges -> return
	if ( optimizer.vertices().size() == 0 )
	{
		pthread_mutex_unlock(&graphMtx);
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "Graph is empty");
		return 0;
	}

	// We need to initialize optimization -> creating jacobians and so on
	int res = optimizer.initializeOptimization();

	// Performing optimization
	res = optimizer.optimize(iterationCount);

	// We extract new position of vertices from optimized edges
	std::set<g2o::OptimizableGraph::Vertex*,
			g2o::OptimizableGraph::VertexIDCompare> verticesToCopy;
	extractVerticesEstimates(verticesToCopy);

	// Unlock the graph
	pthread_mutex_unlock(&graphMtx);

	// We update our estimates based on obtained measurements
	pthread_mutex_lock(&verticesMtx);
	updateVerticesEstimates(verticesToCopy);
	pthread_mutex_unlock(&verticesMtx);


	// TODO: return it to java
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK: Chi2 [%f]",
						optimizer.chi2());

	return res;
}


int GraphManager::saveOptimizationResult(ofstream &ofs) {
	int res = optimizer.save(ofs);
	return res;
}

void GraphManager::delayedAddToGraph(string dataToProcess) {
	pthread_mutex_lock(&addMtx);
	dataToAdd = dataToAdd + "\n" + dataToProcess;
	pthread_mutex_unlock(&addMtx);
}

void GraphManager::addToGraph() {
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "GraphManager::addToGraph");

	// Convert string to stream to process line by line - need to get mutex
	pthread_mutex_lock(&addMtx);
	std::istringstream f(dataToAdd);
	dataToAdd="";
	pthread_mutex_unlock(&addMtx);

	// Locking the graph
	pthread_mutex_lock(&graphMtx);

	// Get a line from data to add
	std::string line;
	while (std::getline(f, line)) {
		stringstream data(line);

		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK: Adding: [%s]",
				data.str().c_str());

		// Getting operation type
		string type;
		data >> type;

		// Adding vertex/edge
		if (type == "EDGE_SE2:WIFI")
			addEdgeWiFi(data);
		else if (type == "EDGE_SE2:STEP")
			addEdgeStepometer(data);
		else if (type == "EDGE_SE2")
			addEdgeSE2(data);
		else if (type == "EDGE_SE2:QR")
			addEdgeQR(data);
		else if (type == "EDGE_SE2:WIFI_FINGERPRINT")
			addVicinityEdge(data, "WiFi Fingerprint");
		else if (type == "EDGE_SE2:VPR_VICINITY")
			addVicinityEdge(data, "VPR Vicinity");
		else if (type == "VERTEX_SE2")
			addVertex(data, 0);
		else if (type == "VERTEX_XY")
			addVertex(data, 1);

		// If we added first edge to fixed nodes, we unfix position 0 so the whole graph can move
		if ( type == "EDGE_SE2:WIFI_FINGERPRINT" || type == "EDGE_SE2:VPR_VICINITY") {
			g2o::OptimizableGraph::Vertex* v = optimizer.vertex(0);
			if (v)
				v->setFixed(false);
		}
	}

	// Unlocking the graph
	pthread_mutex_unlock(&graphMtx);
}

// Get information about position of vertex with given id
std::vector<double> GraphManager::getVertexPosition(int id) {
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK: Called getVertexPosition");

	// locking the vertex mtx
	pthread_mutex_lock(&verticesMtx);

	// Copying data to return it to java
	std::vector<double> estimate;
	for (int i=0;i<vertices.size();i++) {
		if (vertices[i]->vertexId == id)
		{
			estimate.push_back(id);
			if ( vertices[i]->type == ail::Vertex::Type::VERTEX2D) {
				ail::Vertex2D* vertex = static_cast<ail::Vertex2D*> (vertices[i]);
				estimate.push_back(vertex->pos[0]);
				estimate.push_back(vertex->pos[1]);
			}
			else if (vertices[i]->type == ail::Vertex::Type::VERTEXSE2) {
				ail::VertexSE2* vertex = static_cast<ail::VertexSE2*>(vertices[i]);
				estimate.push_back(vertex->pos[0]);
				estimate.push_back(vertex->pos[1]);
				estimate.push_back(vertex->orient);
			}
		}
	}

	// unlocking the vertex mtx
	pthread_mutex_unlock(&verticesMtx);

	return estimate;
}

// Get information about position of all vertices
std::vector<double> GraphManager::getPositionOfAllVertices() {
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG,
			"NDK: Called getPositionOfAllVertices - size: %d", vertices.size());

	// locking the vertex mtx
	pthread_mutex_lock(&verticesMtx);

	// Copying data to return it to java
	std::vector<double> estimate;
	for (int i = 0; i < vertices.size(); i++) {
		estimate.push_back(vertices[i]->vertexId);
		if (vertices[i]->type == ail::Vertex::Type::VERTEX2D) {
			ail::Vertex2D* vertex = static_cast<ail::Vertex2D*>(vertices[i]);
			estimate.push_back(vertex->pos[0]);
			estimate.push_back(vertex->pos[1]);
			estimate.push_back(0);

		} else if (vertices[i]->type == ail::Vertex::Type::VERTEXSE2) {
			ail::VertexSE2* vertex = static_cast<ail::VertexSE2*>(vertices[i]);
			estimate.push_back(vertex->pos[0]);
			estimate.push_back(vertex->pos[1]);
			estimate.push_back(vertex->orient);
		}

	}

	// Creating some prediction
	pthread_mutex_lock(&addMtx);
	std::istringstream f(dataToAdd);
	pthread_mutex_unlock(&addMtx);

	// Get a line from data to add
	std::string line;
	while (std::getline(f, line)) {
		stringstream tmpStream(line);

		// Getting operation type
		string type;
		tmpStream >> type;

		// Adding vertex/edge
		if (type == "EDGE_SE2:STEP")
		{
			int id1, id2;
			double distance, theta;
			tmpStream >> id1 >> id2 >> distance >> theta;

			// Find previous
			for (int i=estimate.size()-4;i>0;i=i-4) {
				if ( estimate[i] == id1) {
					double X = estimate[i+1];
					double Y = estimate[i+2];
					double angle = estimate[i+3];

					angle = angle + theta;
					X = X + distance * cos(angle);
					Y = Y + distance * sin(angle);

					estimate.push_back(id2);
					estimate.push_back(X);
					estimate.push_back(Y);
					estimate.push_back(angle);

					__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG,
													"NDK: Prediction: %d %f %f %f", id2, X, Y, angle);
					break;
				}
			}
		}
	}
	// unlocking the vertex mtx
	pthread_mutex_unlock(&verticesMtx);

	return estimate;
}

void GraphManager::extractVerticesEstimates(
		std::set<g2o::OptimizableGraph::Vertex*,
				g2o::OptimizableGraph::VertexIDCompare> & verticesToCopy) {
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "GraphManager::extractVerticesEstimates");

	for (g2o::HyperGraph::EdgeSet::const_iterator it =
			optimizer.edges().begin(); it != optimizer.edges().end(); ++it) {
		g2o::OptimizableGraph::Edge* e =
				static_cast<g2o::OptimizableGraph::Edge*>(*it);
		if (e->level() == 0) {
			for (std::vector<g2o::HyperGraph::Vertex*>::const_iterator it =
					e->vertices().begin(); it != e->vertices().end(); ++it) {
				g2o::OptimizableGraph::Vertex* v =
						static_cast<g2o::OptimizableGraph::Vertex*>(*it);
				//__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK: Vertex id: %d and fixed: %d", v->id(), v->fixed());
				if (!v->fixed())
					verticesToCopy.insert(
							static_cast<g2o::OptimizableGraph::Vertex*>(*it));
			}
		}
	}
}

void GraphManager::updateVerticesEstimates(
		const std::set<g2o::OptimizableGraph::Vertex*,
				g2o::OptimizableGraph::VertexIDCompare>& verticesToCopy) {
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "GraphManager::updateVerticesEstimates");

	for (auto &v : verticesToCopy) {
		std::vector<double> estimate;
		v->getEstimateData(estimate);

		int index = findIndexInVertices(v->id());

		if (index < 0) {
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG,
					"NDK: Wanted to update the estimates, but there is no vertex with wanted id");
		}
		if (vertices[index]->type == ail::Vertex::Type::VERTEX2D) {
			ail::Vertex2D* vertex = static_cast<ail::Vertex2D*>(vertices[index]);
			vertex->pos[0] = estimate[0];
			vertex->pos[1] = estimate[1];
		} else if (vertices[index]->type == ail::Vertex::Type::VERTEXSE2) {
			ail::VertexSE2* vertex =
					static_cast<ail::VertexSE2*>(vertices[index]);
			vertex->pos[0] = estimate[0];
			vertex->pos[1] = estimate[1];
			vertex->orient = estimate[2];
		}
	}
}


int GraphManager::addVertex(stringstream &data, int type) {

	g2o::OptimizableGraph::Vertex* v;
	ail::Vertex *ailVertex;

	if (type == 0)
	{
		v = new VertexSE2();
		ailVertex = new ail::VertexSE2();
		ailVertex->type = ail::Vertex::Type::VERTEXSE2;
	}
	else if (type == 1)
	{
		v = new VertexPointXY();
		ailVertex = new ail::Vertex2D();
		ailVertex->type = ail::Vertex::Type::VERTEX2D;
	}
	int id;
	data >> id;
	ailVertex->vertexId = id;

	if (id == 0 || id >= 10000) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG,
				"NDK: setFixed(TRUE)!");
		v->setFixed(true);
	}

	bool r = v->read(data);
	if (!r) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]",
				"Error reading vertex!");
		return -1;
	}
	v->setId(id);
	r = optimizer.addVertex(v);
	if (!r) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]",
				"Failure adding Vertex!");
		return -1;
	}

//	pthread_mutex_lock(&bufferMtx);
//	bufferVertices.add(v);
//	pthread_mutex_unlock(&bufferMtx);

	// Adding to vertices
	pthread_mutex_lock(&verticesMtx);
	vertices.push_back(ailVertex);
	pthread_mutex_unlock(&verticesMtx);
}

int GraphManager::addVicinityEdge(stringstream &data, string name)
{
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [Adding %s edge]", name.c_str());
	int id1, id2;
	EdgeSE2PlaceVicinity* e = new EdgeSE2PlaceVicinity();

	data >> id1 >> id2;
	OptimizableGraph::Vertex* from = optimizer.vertex(id1);
	OptimizableGraph::Vertex* to = optimizer.vertex(id2);

	if (from && to) {
		e->setVertex(0, from);
		e->setVertex(1, to);
		e->read(data);
//		pthread_mutex_lock(&bufferMtx);
//		bufferPlaceVicinityEdges.push_back(e);
//		pthread_mutex_unlock(&bufferMtx);
		if (!optimizer.addEdge(e)) {
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [Unable to add edge %s]", name.c_str());
			delete e;
			return -1;
		}
	}
	return 0;
}

int GraphManager::addEdgeWiFi(stringstream &data) {
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]",
					"Adding wifi edge");
	int id1, id2;
	EdgeSE2PointXYDistance* e = new EdgeSE2PointXYDistance();
	data >> id1 >> id2;
	OptimizableGraph::Vertex* from = optimizer.vertex(id1);
	OptimizableGraph::Vertex* to = optimizer.vertex(id2);
	if (!to) {
		stringstream tmp;
		tmp << id2 << " " << prevUserPositionX << " " << prevUserPositionY
				<< "\n";
		addVertex(tmp, 1);
		to = optimizer.vertex(id2);
	}

	if (from && to) {
		e->setVertex(0, from);
		e->setVertex(1, to);
		e->read(data);
//		pthread_mutex_lock(&bufferMtx);
//		bufferPointXYDistanceEdges.push_back(e);
//		pthread_mutex_unlock(&bufferMtx);
		if (!optimizer.addEdge(e)) {
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]",
					"Unable to add edge wifi");
			delete e;
			return -1;
		}
	}
	return 0;
}
int GraphManager::addEdgeStepometer(stringstream &data) {
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]",
					"Adding stepometer edge");

	int id1, id2;
	EdgeSE2DistanceOrientation* e = new EdgeSE2DistanceOrientation();
	data >> id1 >> id2;
	OptimizableGraph::Vertex* from = optimizer.vertex(id1);
	OptimizableGraph::Vertex* to = optimizer.vertex(id2);
	if (!from) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]",
				"Adding initial vertex");
		stringstream tmp;
		tmp << id1 << " 0.0 0.0 0.0\n";
		addVertex(tmp, 0);
		from = optimizer.vertex(id1);
	}

	if (!to) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]",
				"Adding new vertex");
		stringstream tmpStream(data.str());
		double distance, theta;
		string xxx;
		tmpStream >> xxx >> id1 >> id2 >> distance >> theta;
		prevUserPositionTheta = prevUserPositionTheta + theta;
		prevUserPositionX = prevUserPositionX
				+ distance * cos(prevUserPositionTheta);
		prevUserPositionY = prevUserPositionY
				+ distance * sin(prevUserPositionTheta);

		stringstream tmp;
		tmp << id2 << " " << prevUserPositionX << " " << prevUserPositionY
				<< " " << prevUserPositionTheta << " \n";
		addVertex(tmp, 0);
		to = optimizer.vertex(id2);
	}

	if (from && to) {
		e->setVertex(0, from);
		e->setVertex(1, to);
		e->read(data);
//		pthread_mutex_lock(&bufferMtx);
//		bufferStepometerEdges.push_back(e);
//		pthread_mutex_unlock(&bufferMtx);
		if (!optimizer.addEdge(e)) {
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]",
					"Unable to add edge stepometer");
			delete e;
			return -1;
		}
	}
	return 0;
}


int GraphManager::addEdgeSE2(stringstream &data) {
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]",
					"Adding SE2 edge");

	int id1, id2;
	EdgeSE2* e = new EdgeSE2();
	data >> id1 >> id2;
	OptimizableGraph::Vertex* from = optimizer.vertex(id1);
	OptimizableGraph::Vertex* to = optimizer.vertex(id2);
	if (!from) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]",
				"Adding initial vertex");
		stringstream tmp;
		tmp << id1 << " 0.0 0.0 0.0\n";
		addVertex(tmp, 0);
		from = optimizer.vertex(id1);
	}

	if (!to) {
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]",
				"Adding new vertex");
		stringstream tmpStream(data.str());
		double distX, distY, theta;
		string xxx;
		tmpStream >> xxx >> id1 >> id2 >> distX >> distY >> theta;

		prevUserPositionX = prevUserPositionX + cos(prevUserPositionTheta) * distX - sin(prevUserPositionTheta) * distY;
		prevUserPositionY = prevUserPositionY + sin(prevUserPositionTheta) * distX + cos(prevUserPositionTheta) * distY;

		prevUserPositionTheta = prevUserPositionTheta + theta;

		stringstream tmp;
		tmp << id2 << " " << prevUserPositionX << " " << prevUserPositionY
				<< " " << prevUserPositionTheta << " \n";
		addVertex(tmp, 0);
		to = optimizer.vertex(id2);
	}

	if (from && to) {
		e->setVertex(0, from);
		e->setVertex(1, to);
		e->read(data);
		if (!optimizer.addEdge(e)) {
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]",
					"Unable to add edge SE2");
			delete e;
			return -1;
		}
	}
	return 0;
}

int GraphManager::addEdgeQR(stringstream &data) {
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [%s]",
					"Adding SE2 QR");

	int id1, id2;
	EdgeSE2QR* e = new EdgeSE2QR();
	data >> id1 >> id2;
	OptimizableGraph::Vertex* from = optimizer.vertex(id1);
	OptimizableGraph::Vertex* to = optimizer.vertex(id2);
	if (from && to) {
		e->setVertex(0, from);
		e->setVertex(1, to);
		e->read(data);
//		pthread_mutex_lock(&bufferMtx);
//		bufferSE2QR.push_back(e);
//		thread_mutex_unlock(&bufferMtx);
		if (!optimizer.addEdge(e)) {
			__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "NDK:LC: [Unable to add edge QR]");
			delete e;
			return -1;
		}
	}
	return 0;
}

int GraphManager::findIndexInVertices(int id) {
	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i]->vertexId == id)
			return i;
	}
	return -1;
}
