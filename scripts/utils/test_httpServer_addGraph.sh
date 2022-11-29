#!/bin/bash
curl -H "Content-Type: application/json" -X POST -d@postScript2.json "http://localhost:8080/add_graph"