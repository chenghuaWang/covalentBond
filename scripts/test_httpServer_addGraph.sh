#!/bin/bash
curl -H "Content-Type: application/json" -X POST -d@postScript.json "http://localhost:8080/add_graph"