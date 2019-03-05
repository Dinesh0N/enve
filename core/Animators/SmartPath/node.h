#ifndef NODE_H
#define NODE_H
#include "pointhelpers.h"
#include "exceptions.h"

struct Node {
    friend class NodeList;
    enum Type : char {
        DUMMY, DISSOLVED, NORMAL, MOVE
    };

    Node() : Node(DUMMY) {}

    Node(const Type& type) { fType = type; }

    Node(const QPointF& c0, const QPointF& p1, const QPointF& c2) {
        fC0 = c0;
        fP1 = p1;
        fC2 = c2;
        fType = NORMAL;
    }

    Node(const qreal& t) {
        fT = t;
        fType = DISSOLVED;
    }

    bool isMove() const { return fType == MOVE; }

    bool isNormal() const { return fType == NORMAL; }

    bool isDummy() const { return fType == DUMMY; }

    bool isDissolved() const { return fType == DISSOLVED; }

    const int &getNextNodeId() const {
        return fNextNodeId;
    }

    const int &getPrevNodeId() const {
        return fPrevNodeId;
    }

    const int &getNodeId() const {
        return fId;
    }

    bool hasPreviousNode() const {
        return getPrevNodeId() >= 0;
    }

    bool hasNextNode() const {
        return getNextNodeId() >= 0;
    }

    QPointF fC0;
    QPointF fP1;
    QPointF fC2;

    //! @brief T value for segment defined by previous and next normal node
    qreal fT;

    const Type& getType() const { return fType; }
    const CtrlsMode& getCtrlsMode() const { return fCtrlsMode; }
    const bool& getC0Enabled() const {
        return fC0Enabled;
    }

    const bool& getC2Enabled() const {
        return fC2Enabled;
    }

    static Node sInterpolateNormal(const Node& node1, const Node& node2,
                                   const qreal &weight2);

    bool operator==(const Node& other) const {
        if(other.getType() != getType()) return false;
        if(isDummy() || isMove()) return true;
        if(getPrevNodeId() != other.getPrevNodeId()) return false;
        if(getNextNodeId() != other.getNextNodeId()) return false;
        if(isDissolved()) return isZero6Dec(fT - other.fT);
        if(isNormal()) {
            if(getC0Enabled() != other.getC0Enabled()) return false;
            if(!isZero6Dec(pointToLen(fC0 - other.fC0))) return false;
            if(!isZero6Dec(pointToLen(fP1 - other.fP1))) return false;
            if(getC2Enabled() != other.getC2Enabled()) return false;
            if(!isZero6Dec(pointToLen(fC2 - other.fC2))) return false;
        }
        return false;
    }

    bool operator!=(const Node& other) const {
        return !this->operator==(other);
    }
protected:
    void switchPrevAndNext() {
        const int prevT = fPrevNodeId;
        fPrevNodeId = fNextNodeId;
        fNextNodeId = prevT;
    }

    void shiftIdsGreaterThan(const int& greater, const int& shiftBy) {
        if(fId > greater) fId += shiftBy;
        if(fPrevNodeId) if(fPrevNodeId > greater) fPrevNodeId += shiftBy;
        if(fNextNodeId) if(fNextNodeId > greater) fNextNodeId += shiftBy;
    }

    void setPrevNodeId(const int& prevNodeId) {
        fPrevNodeId = prevNodeId;
    }

    void setNextNodeId(const int& nextNodeId) {
        fNextNodeId = nextNodeId;
    }

    void setNodeId(const int& nodeId) {
        fId = nodeId;
    }

    void setType(const Type& type) {
        fType = type;
    }

    void setCtrlsMode(const CtrlsMode& ctrlsMode) {
        fCtrlsMode = ctrlsMode;
        if(ctrlsMode == CtrlsMode::CTRLS_SYMMETRIC) {
            gGetCtrlsSymmetricPos(fC0, fP1, fC2, fC0, fC2);
        } else if(ctrlsMode == CtrlsMode::CTRLS_SMOOTH) {
            gGetCtrlsSmoothPos(fC0, fP1, fC2, fC0, fC2);
        } else return;
        setC0Enabled(true);
        setC2Enabled(true);
    }

    void setC0Enabled(const bool& enabled) {
        fC0Enabled = enabled;
    }

    void setC2Enabled(const bool& enabled) {
        fC2Enabled = enabled;
    }

    bool fC0Enabled = true;
    bool fC2Enabled = true;
    Type fType;
    CtrlsMode fCtrlsMode = CtrlsMode::CTRLS_SYMMETRIC;

    int fId = -1;
    //! @brief Previous connected node id in the list.
    int fPrevNodeId = -1;
    //! @brief Next connected node id in the list.
    int fNextNodeId = -1;
};
#include "smartPointers/stdselfref.h"
class ListOfNodes {
public:
    void append(const Node& nodeBlueprint) {
        insert(mList.count(), nodeBlueprint);
    }

    void insert(const int& id, const Node& nodeBlueprint) {
        Node * const newNode = insertNewNode(id);
        *newNode = nodeBlueprint;
    }

    void clear() {
        mList.clear();
    }

    Node* at(const int& id) const {
        if(id < 0 || id >= count())
            RuntimeThrow("Index out of range.");
        return mList.at(id).get();
    }

    int count() const {
        return mList.count();
    }

    ListOfNodes& operator=(const ListOfNodes& other) {
        const int otherCount = other.count();
        const int thisCount = count();
        int i = 0;
        for(; i < otherCount && i < thisCount; i++) {
            assign(i, *other.at(i));
        }
        for(; i < otherCount; i++) {
            insert(i, *other.at(i));
        }
        for(; i < thisCount; i++) {
            removeAt(i);
        }
        return *this;
    }

    Node * operator[](const int& id) const {
        return at(id);
    }

    void removeAt(const int& id) {
        mList.removeAt(id);
    }

    void assign(const int& id, const Node& nodeBlueprint) {
        if(id < 0 || id >= count())
            RuntimeThrow("Index out of range.");
        *mList.at(id).get() = nodeBlueprint;
    }

    auto begin() const {
        return mList.begin();
    }

    auto end() const {
        return mList.end();
    }
private:
    Node* insertNewNode(const int& id) {
        const auto newNode = stdsptr<Node>(new Node);
        mList.insert(id, newNode);
        return newNode.get();
    }
    QList<stdsptr<Node>> mList;
};

#endif // NODE_H
