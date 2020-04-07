project = CreateProject()

target = project:CreateLibrary("text_utils", STATIC | SHARED)
target:AddSourceFiles("*.cpp")

return project
