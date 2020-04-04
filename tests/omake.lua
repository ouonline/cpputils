project = CreateProject()

target = project:CreateBinary("test_text_utils")
target:AddSourceFile("*.cpp")
target:AddStaticLibrary("..", "text_utils")

return project
