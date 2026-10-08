pipeline {
    agent any
    stages {
        stage('Build') {
            steps {
                sh 'cmake -S . -B build-jenkins'
                sh 'cmake --build build-jenkins'
            }
        }
        stage('Test') {
            steps {
                sh './build-jenkins/unit_tests --gtest_output=xml:build-jenkins/test-results.xml'
            }
            post {
                always {
                    junit 'build-jenkins/test-results.xml'
                }
            }
        }
    }
}