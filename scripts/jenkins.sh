#!/usr/bin/env bash
# Start a local Jenkins at http://localhost:8080 that builds this repository.
# Usage: ./scripts/jenkins.sh        stop with: docker stop j1939-jenkins
set -euo pipefail

REPO="$(cd "$(dirname "$0")/.." && pwd)"

docker build -t j1939-jenkins "$REPO/docker/jenkins"

docker rm -f j1939-jenkins >/dev/null 2>&1 || true
docker run -d --name j1939-jenkins \
    -p 127.0.0.1:8080:8080 \
    -v j1939-jenkins-home:/var/jenkins_home \
    -v "$REPO":/repo:ro \
    -e JAVA_OPTS="-Djenkins.install.runSetupWizard=false -Dhudson.plugins.git.GitSCM.ALLOW_LOCAL_CHECKOUT=true" \
    j1939-jenkins

echo "Jenkins is starting: open http://localhost:8080 in about a minute"
