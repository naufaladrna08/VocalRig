#define DEBUG
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <misc/cpp/imgui_stdlib.h>
#include <vector>
#include <algorithm>
#include "Theme.hpp"
#include <ImGuizmo.h>
#include <ImCurveEdit.h>
#include <ImSequencer.h>
#include <example/imgui_internal.h>

static const char* SequencerItemTypeNames[] = { "Camera","Music", "ScreenEffect", "FadeIn", "Animation" };

struct RampEdit : public ImCurveEdit::Delegate
{
   RampEdit()
   {
      mPts[0][0] = ImVec2(-10.f, 0);
      mPts[0][1] = ImVec2(20.f, 0.6f);
      mPts[0][2] = ImVec2(25.f, 0.2f);
      mPts[0][3] = ImVec2(70.f, 0.4f);
      mPts[0][4] = ImVec2(120.f, 1.f);
      mPointCount[0] = 5;

      mPts[1][0] = ImVec2(-50.f, 0.2f);
      mPts[1][1] = ImVec2(33.f, 0.7f);
      mPts[1][2] = ImVec2(80.f, 0.2f);
      mPts[1][3] = ImVec2(82.f, 0.8f);
      mPointCount[1] = 4;


      mPts[2][0] = ImVec2(40.f, 0);
      mPts[2][1] = ImVec2(60.f, 0.1f);
      mPts[2][2] = ImVec2(90.f, 0.82f);
      mPts[2][3] = ImVec2(150.f, 0.24f);
      mPts[2][4] = ImVec2(200.f, 0.34f);
      mPts[2][5] = ImVec2(250.f, 0.12f);
      mPointCount[2] = 6;
      mbVisible[0] = mbVisible[1] = mbVisible[2] = true;
      mMax = ImVec2(1.f, 1.f);
      mMin = ImVec2(0.f, 0.f);
   }
   size_t GetCurveCount()
   {
      return 3;
   }

   bool IsVisible(size_t curveIndex)
   {
      return mbVisible[curveIndex];
   }
   size_t GetPointCount(size_t curveIndex)
   {
      return mPointCount[curveIndex];
   }

   uint32_t GetCurveColor(size_t curveIndex)
   {
      uint32_t cols[] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };
      return cols[curveIndex];
   }
   ImVec2* GetPoints(size_t curveIndex)
   {
      return mPts[curveIndex];
   }
   virtual ImCurveEdit::CurveType GetCurveType(size_t curveIndex) const { return ImCurveEdit::CurveSmooth; }
   virtual int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value)
   {
      mPts[curveIndex][pointIndex] = ImVec2(value.x, value.y);
      SortValues(curveIndex);
      for (size_t i = 0; i < GetPointCount(curveIndex); i++)
      {
         if (mPts[curveIndex][i].x == value.x)
            return (int)i;
      }
      return pointIndex;
   }
   virtual void AddPoint(size_t curveIndex, ImVec2 value)
   {
      if (mPointCount[curveIndex] >= 8)
         return;
      mPts[curveIndex][mPointCount[curveIndex]++] = value;
      SortValues(curveIndex);
   }
   virtual ImVec2& GetMax() { return mMax; }
   virtual ImVec2& GetMin() { return mMin; }
   virtual unsigned int GetBackgroundColor() { return 0; }
   ImVec2 mPts[3][8];
   size_t mPointCount[3];
   bool mbVisible[3];
   ImVec2 mMin;
   ImVec2 mMax;
private:
   void SortValues(size_t curveIndex)
   {
      auto b = std::begin(mPts[curveIndex]);
      auto e = std::begin(mPts[curveIndex]) + GetPointCount(curveIndex);
      std::sort(b, e, [](ImVec2 a, ImVec2 b) { return a.x < b.x; });

   }
};

struct MySequence : public ImSequencer::SequenceInterface
{
   // interface with sequencer

   virtual int GetFrameMin() const {
      return mFrameMin;
   }
   virtual int GetFrameMax() const {
      return mFrameMax;
   }
   virtual int GetItemCount() const { return (int)myItems.size(); }

   virtual int GetItemTypeCount() const { return sizeof(SequencerItemTypeNames) / sizeof(char*); }
   virtual const char* GetItemTypeName(int typeIndex) const { return SequencerItemTypeNames[typeIndex]; }
   virtual const char* GetItemLabel(int index) const
   {
      static char tmps[512];
      snprintf(tmps, 512, "[%02d] %s", index, SequencerItemTypeNames[myItems[index].mType]);
      return tmps;
   }

   virtual void Get(int index, int** start, int** end, int* type, unsigned int* color)
   {
      MySequenceItem& item = myItems[index];
      if (color)
         *color = 0xFFAA8080; // same color for everyone, return color based on type
      if (start)
         *start = &item.mFrameStart;
      if (end)
         *end = &item.mFrameEnd;
      if (type)
         *type = item.mType;
   }
   virtual void Add(int type) { myItems.push_back(MySequenceItem{ type, 0, 10, false }); };
   virtual void Del(int index) { myItems.erase(myItems.begin() + index); }
   virtual void Duplicate(int index) { myItems.push_back(myItems[index]); }

   virtual size_t GetCustomHeight(int index) { return myItems[index].mExpanded ? 300 : 0; }

   // my datas
   MySequence() : mFrameMin(0), mFrameMax(0) {}
   int mFrameMin, mFrameMax;
   struct MySequenceItem
   {
      int mType;
      int mFrameStart, mFrameEnd;
      bool mExpanded;
   };
   std::vector<MySequenceItem> myItems;
   RampEdit rampEdit;

   virtual void DoubleClick(int index) {
      if (myItems[index].mExpanded)
      {
         myItems[index].mExpanded = false;
         return;
      }
      for (auto& item : myItems)
         item.mExpanded = false;
      myItems[index].mExpanded = !myItems[index].mExpanded;
   }

   virtual void CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect)
   {
      static const char* labels[] = { "Translation", "Rotation" , "Scale" };

      rampEdit.mMax = ImVec2(float(mFrameMax), 1.f);
      rampEdit.mMin = ImVec2(float(mFrameMin), 0.f);
      draw_list->PushClipRect(legendClippingRect.Min, legendClippingRect.Max, true);
      for (int i = 0; i < 3; i++)
      {
         ImVec2 pta(legendRect.Min.x + 30, legendRect.Min.y + i * 14.f);
         ImVec2 ptb(legendRect.Max.x, legendRect.Min.y + (i + 1) * 14.f);
         draw_list->AddText(pta, rampEdit.mbVisible[i] ? 0xFFFFFFFF : 0x80FFFFFF, labels[i]);
         if (ImRect(pta, ptb).Contains(ImGui::GetMousePos()) && ImGui::IsMouseClicked(0))
            rampEdit.mbVisible[i] = !rampEdit.mbVisible[i];
      }
      draw_list->PopClipRect();

      ImGui::SetCursorScreenPos(rc.Min);
      ImVec2 newSize;
      newSize.x = rc.Max.x - rc.Min.x;
      newSize.y = rc.Max.y - rc.Min.y;
      ImCurveEdit::Edit(rampEdit, newSize, 137 + index, &clippingRect);
   }

   virtual void CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect)
   {
      rampEdit.mMax = ImVec2(float(mFrameMax), 1.f);
      rampEdit.mMin = ImVec2(float(mFrameMin), 0.f);
      draw_list->PushClipRect(clippingRect.Min, clippingRect.Max, true);
      for (int i = 0; i < 3; i++)
      {
         for (int j = 0; j < rampEdit.mPointCount[i]; j++)
         {
            float p = rampEdit.mPts[i][j].x;
            if (p < myItems[index].mFrameStart || p > myItems[index].mFrameEnd)
               continue;
            float r = (p - mFrameMin) / float(mFrameMax - mFrameMin);
            float x = ImLerp(rc.Min.x, rc.Max.x, r);
            draw_list->AddLine(ImVec2(x, rc.Min.y + 6), ImVec2(x, rc.Max.y - 4), 0xAA000000, 4.f);
         }
      }
      draw_list->PopClipRect();
   }
};


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

int main(int argc, char** argv) {
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << std::endl;
    return -1;
  }  

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

  GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL", NULL, NULL);
  if (!window) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glfwSetKeyCallback(window, key_callback);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  ImGuiTheme();
  io.Fonts->AddFontFromFileTTF("../assets/fonts/Ubuntu-Medium.ttf", 16.0f);

  bool p_open = true;
  double mouseX, mouseY;
  glfwGetCursorPos(window, &mouseX, &mouseY);

  MySequence mySequence;
  mySequence.mFrameMin = -100;
  mySequence.mFrameMax = 1000;
  mySequence.myItems.push_back(MySequence::MySequenceItem{ 0, 10, 30, false });
  mySequence.myItems.push_back(MySequence::MySequenceItem{ 1, 20, 30, true });
  mySequence.myItems.push_back(MySequence::MySequenceItem{ 3, 12, 60, false });
  mySequence.myItems.push_back(MySequence::MySequenceItem{ 2, 61, 90, false });
  mySequence.myItems.push_back(MySequence::MySequenceItem{ 4, 90, 99, false });

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    ImGui::Begin("DockSpace Demo", &p_open, window_flags);
      ImGui::PopStyleVar();

      ImGuiID dockspace_id = ImGui::GetID("dock");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          if (ImGui::MenuItem("New")) {}
          if (ImGui::MenuItem("Open")) {}
          if (ImGui::MenuItem("Save")) {}
          if (ImGui::MenuItem("Save As")) {}
          if (ImGui::MenuItem("Exit")) {}
          ImGui::End();
        }

        ImGui::EndMenuBar();
      }

      ImGui::ShowDemoWindow();

      // let's create the sequencer
      ImGui::Begin("Sequencer");
      static int selectedEntry = -1;
      static int firstFrame = 0;
      static bool expanded = true;
      static int currentFrame = 100;

      ImGui::PushItemWidth(130);
      ImGui::InputInt("Frame Min", &mySequence.mFrameMin);
      ImGui::SameLine();
      ImGui::InputInt("Frame ", &currentFrame);
      ImGui::SameLine();
      ImGui::InputInt("Frame Max", &mySequence.mFrameMax);
      ImGui::PopItemWidth();
      Sequencer(&mySequence, &currentFrame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_CHANGE_FRAME);
      // add a UI to edit that particular item
      if (selectedEntry != -1) {
        const MySequence::MySequenceItem &item = mySequence.myItems[selectedEntry];
        ImGui::Text("I am a %s, please edit me", SequencerItemTypeNames[item.mType]);
        // switch (type) ....
      }
      ImGui::End();

    ImGui::End();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
