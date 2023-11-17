#pragma once
class DigitBehavior : public CustomComponent
{
public:
	DigitBehavior();
	~DigitBehavior();

	void Awake() override;
	void Start() override;
	void Update() override;

	void SetDigit(UINT digit);
	float CalculateUVOffsetFromDigit();

private:
	VGUiRenderer* m_uiRenderer;

	bool m_isDigitDirty;

	UINT m_digit;
	float m_prevOffsetY;
};

