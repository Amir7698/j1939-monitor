// Runs at Jenkins start-up: creates a pipeline job that reads the Jenkinsfile
// from the project repository mounted at /repo (branch main).
import jenkins.model.Jenkins
import org.jenkinsci.plugins.workflow.job.WorkflowJob
import org.jenkinsci.plugins.workflow.cps.CpsScmFlowDefinition
import hudson.plugins.git.GitSCM
import hudson.plugins.git.BranchSpec
import hudson.plugins.git.UserRemoteConfig

def jenkins = Jenkins.get()
if (jenkins.getItem('j1939-monitor') == null) {
    def scm = new GitSCM(
        [new UserRemoteConfig('file:///repo', null, null, null)],
        [new BranchSpec('*/main')],
        null, null, [])
    def job = jenkins.createProject(WorkflowJob, 'j1939-monitor')
    job.definition = new CpsScmFlowDefinition(scm, 'Jenkinsfile')
    job.save()
}
