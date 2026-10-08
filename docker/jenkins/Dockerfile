# Local Jenkins for j1939-monitor: Jenkins LTS plus the C++ build tools.
FROM jenkins/jenkins:lts-jdk17

USER root
RUN apt-get update \
 && apt-get install -y --no-install-recommends g++ cmake ninja-build libgtest-dev python3 \
 && rm -rf /var/lib/apt/lists/* \
 && git config --system --add safe.directory '*'
USER jenkins

# Pipeline support, Git checkout, JUnit test reports, stage view.
RUN jenkins-plugin-cli --plugins workflow-aggregator git junit pipeline-stage-view

# Create the "j1939-monitor" pipeline job on first start.
COPY create-job.groovy /usr/share/jenkins/ref/init.groovy.d/create-job.groovy
