project = CreateProject()

target = project:CreateLibrary("text_utils")
target:AddSourceFile("*.cpp")

return project
