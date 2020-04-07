project = CreateProject()

target = project:CreateBinary("test_text_utils")
target:AddSourceFiles("*.cpp")
target:AddLibrary("..", "text_utils", STATIC)

return project
