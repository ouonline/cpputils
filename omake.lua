project = CreateProject()

target = project:CreateLibrary("text_utils", STATIC)
target:AddSourceFiles("*.cpp")

return project
